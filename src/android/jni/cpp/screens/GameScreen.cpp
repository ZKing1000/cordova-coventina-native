// vim: sw=4 expandtab

#include "../../screens/GameScreen.h"

#include <SDL.h>

#include "../../opengl/programs.h"

#include "../../matrix.h"

#include "../../loop.h"

#include "../../extra.h"

#include "../../read.h"

#include "Player.hpp"
#include "Floor.hpp"
#include "Cube.hpp"
#include "CubeMesh.hpp"
#include "Ring.hpp"
#if USE_ANTBAR == 1
#include "AntTweakBar.h"
#endif

using game::player;

bool frozen = false;
unsigned WinScore = 6;

constexpr float DEGRAD = M_PI / 180.;

game::CubeMesh cubeMesh;
glm::ivec2 wellCenter;

GameScreen* GameScreen::instance = new GameScreen();


InputEvent* GameScreen::inputEvent;
Thumbstick* GameScreen::thumbstick = nullptr;
unsigned GameScreen::level = 1;

GameScreen::GameScreen(){

  loop::currentScreen = this;

  inputEvent = new InputEvent();
#ifdef __ANDROID__
  thumbstick = new Thumbstick();
#endif

}

constexpr bool infinitePlay = false;
constexpr bool freezePlayerWhenTimesUp = false;
static unsigned timeleftSeconds = infinitePlay ? 0 : 90;
static char result[10] = "";
bool finished = false;

struct Thing
{
    static void genGraphics();
    void draw();

    static std::vector<Shape*> scene;

    static Shape* shs;

    glm::vec4 lightPos = {0,0,2,1};

} thing;

//static float colorVar[] = { 0., 1., 0. };

#if USE_ANTBAR == 1
TwBar *bar;
#endif

void initUI()
{
#if USE_ANTBAR == 1
    extern float avgFps;

    if (!TwInit(TW_OPENGL_CORE, NULL)) {
      SDL_Log("Failed to init TweakBar: %s\n", TwGetLastError());
      return;
    }

    int width = screen::pixelBounds[0];
    int height = screen::pixelBounds[1];
    TwWindowSize(width, height);
    bar = TwNewBar("Coventina");
    TwDefine(" Coventina size='200 130' help='Is this helpful?\n' ");
    //TwAddVarRW(bar, "Color", TW_TYPE_COLOR3F, &colorVar, " help='color' ");
    TwAddVarRW(bar, "Framerate", TW_TYPE_FLOAT, &avgFps, " help='framerate' ");
    TwAddVarRW(bar, "Coins", TW_TYPE_UINT32, &player.coinCount, " help='coins' ");
    TwAddVarRW(bar, "Rings", TW_TYPE_UINT32, &player.ringCount, " help='rings' ");
    TwAddVarRW(bar, "Time Left", TW_TYPE_UINT32, &timeleftSeconds, " help='seconds remaining' ");
    TwAddVarRW(bar, "Score", TW_TYPE_UINT32, &player.score, " help='score' ");
    TwAddVarRW(bar, "Result", TW_TYPE_CSSTRING(10), &result, " help='results' ");
#endif
}

void genSplash();
void drawSplash();

Sound* GameScreen::pickupSound;
Sound* GameScreen::dropSound;
Sound* GameScreen::jumpSound;
Sound* GameScreen::winnerSound;
Sound* GameScreen::loserSound;

void GameScreen::genGraphics(){

    initUI();

    if (thumbstick)
        thumbstick->genGraphics();

    timeleft = timeleftSeconds;

    pickupSound = audio::sound("select.wav");
    dropSound = audio::sound("drop.wav");
    jumpSound = audio::sound("jump.wav");
    winnerSound = audio::sound("winner.wav");
    loserSound = audio::sound("loser.wav");

    //Bullet::genGraphics();

    genSplash();
    game::Floor::genGraphics();
    game::Cube::genGraphics();
    game::Coin::genGraphics();
    game::Ring::genGraphics();
    CuttleFish::genGraphics();
    Thing::genGraphics();

    cubeMesh.readMap("maze.grid");
    //cubeMesh.readCubes("map.in");
    cubeMesh.addRandoms();
    wellCenter = cubeMesh.wellCenter;
    player.pos[0] = cubeMesh.playerStart[0];
    player.pos[2] = cubeMesh.playerStart[1];

    cuttleFish = new CuttleFish(glm::vec3{wellCenter[0], 4., wellCenter[1]-1.});
}

void GameScreen::keyInput()
{
    SDL_Event event;

    while(SDL_PollEvent(&event)){

      //gun.keyInput(event);

#if USE_ANTBAR == 1
        if (TwEventSDL(&event, SDL_MAJOR_VERSION, SDL_MINOR_VERSION))
            continue;
#endif

        switch(event.type){
        case SDL_QUIT:
            exit(0);
            break;

        case SDL_KEYDOWN:
            switch( event.key.keysym.sym ){
            case SDLK_e:
                player.runForward();
                break;
            case SDLK_d:
                player.runBackward();
                break;
            case SDLK_f:
                player.runLeft();
                break;
            case SDLK_s:
                player.runRight();
                break;
            case SDLK_LEFT:
                player.setRotation(-1);
                break;
            case SDLK_RIGHT:
                player.setRotation(1);
                break;
            case SDLK_UP:
                player.setXRotation(-1);
                break;
            case SDLK_DOWN:
                player.setXRotation(1);
                break;
            case SDLK_SPACE:
                player.jump();
                break;
            case SDLK_ESCAPE:
                player.togglePointerMode();
                SDL_SetRelativeMouseMode(player.pointerMode ? SDL_TRUE : SDL_FALSE);
                break;
            case SDLK_p:
                frozen = !frozen;
                break;
            case SDLK_q:
                exit(0);
                break;
            }
            break;
        case SDL_KEYUP:
            switch( event.key.keysym.sym ){
            case SDLK_e:
                player.stopForward();
                break;
            case SDLK_d:
                player.stopBackward();
                break;
            case SDLK_f:
                player.stopLeft();
                break;
            case SDLK_s:
                player.stopRight();
                break;
            case SDLK_LEFT:
            case SDLK_RIGHT:
                player.setRotation(0);
                break;
            case SDLK_UP:
            case SDLK_DOWN:
                player.setXRotation(0);
                break;
            case SDLK_PAGEUP:
            case SDLK_PAGEDOWN:
                break;
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            mouseButtonDown = true;
            inputEvent->pressed = true;
            break;
        case SDL_MOUSEBUTTONUP:
            mouseButtonDown = false;
            inputEvent->pressed = false;
            break;
        case SDL_MOUSEMOTION:
            inputEvent->update();
            if (player.pointerMode)
            {
                int rx = event.motion.xrel;
                int ry = event.motion.yrel;

                std::array<int,2> pix = {screen::pixelBounds[0]/2, screen::pixelBounds[1]/2};
                float bounds[2] = {screen::bounds[0],screen::bounds[1]};

                float x = rx/float(pix[0]) * bounds[0];
                float y = ry/float(pix[1]) * bounds[1];

                player.setPointerDelta(x, y);
            }

            break;
        case SDL_WINDOWEVENT:
            switch (event.window.event)
            {
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                SDL_Log("Window resized: %dx%d\n",
                        event.window.data1, event.window.data2);
                screen::pixelBounds[0] = event.window.data1;
                screen::pixelBounds[1] = event.window.data2;
                screen::bounds[0] = 1;
                screen::bounds[1] = float(screen::pixelBounds[1])/screen::pixelBounds[0];
                glViewport(0,0,screen::pixelBounds[0],screen::pixelBounds[1]);
                break;
            }
            break;
        }
    }

    if (thumbstick)
        thumbstick->keyInput(event,inputEvent);
}

void GameScreen::render(float dt)
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    mat::clearMatrices();

    mat::perspective(60*DEGRAD,screen::pixelBounds[0]/float(screen::pixelBounds[1]));

    mat::view = player.viewMat;

    game::floor.draw();

    {
        static int cnt = 0;
        if (cnt < 60 * 1)
        {
            drawSplash();
            ++cnt;
            return;
        }
    }

    if (timeleft > 0)
    {
        timeleftSeconds = unsigned(timeleft);
        timeleft -= dt;
    }
    else if (!finished && !infinitePlay)
    {
        finished = true;
        timeleftSeconds = 0;
        if (player.score >= WinScore)
        {
            strcpy(result, "Winner!");
            audio::playSound(winnerSound);
        }
        else
        {
            strcpy(result, "Loser!");
            audio::playSound(loserSound);
        }
        printf("%s\n", result);
    }

    for (auto &cube : cubeMesh.cubes)
    {
        if (cube->valid)
            cube->draw();
    }

    for (auto &coin : cubeMesh.coins)
    {
        if (coin->valid)
            coin->draw();
    }

    for (auto &ring : cubeMesh.rings)
    {
        if (ring->valid)
            ring->draw();
    }

    //gun.draw();

    /*for(int i=0;i<Bullet::instances.size();i++)
      Bullet::instances[i]->draw();*/

    cuttleFish->draw();

    thing.draw();

    mat::clearMatrices();

    if (thumbstick)
        thumbstick->draw();


#if USE_ANTBAR == 1
    TwDraw();
#endif

    if (!frozen)
    {
        offsetX+=0.005;
        offsetY+=0.005;

        if(offsetX>1)
            offsetX = 0;
        if(offsetY>1)
            offsetY = 0;
    }
}

void GameScreen::update(float dt)
{
    keyInput();

    if (thumbstick)
        thumbstick->update();

    if (!frozen)
    {
        if (timeleftSeconds > 0 || !freezePlayerWhenTimesUp)
        {
            player.handleBlocks(cubeMesh, dt);
            player.update(dt);
        }

        game::Coin::update(dt);
        game::Ring::update(dt);
        cuttleFish->update(dt);

        //gun.update();

        /*for(int i=0;i<Bullet::instances.size();i++)
          Bullet::instances[i]->update();*/

    }

#if USE_ANTBAR == 1
    TwRefreshBar(bar);
#endif

    render(dt);

}

std::vector<Shape*> Thing::scene;
Shape* Thing::shs;

void Thing::genGraphics()
{
    //scene = read::holden("WithNormals.holden");
    scene = read::holden("Ring.holden");

    shs = 0;
    if (true)
    {
        shs = new Shape();

        shs->verts = scene[0]->verts;
        shs->indices = scene[0]->indices;
        shs->colors = std::vector<ColorRGB>(1);
        shs->colors[0] = ColorRGB(1,0,0);

        SDL_Log(("Verts Size: "+toString(shs->verts.size())+" indices size: "+toString(shs->indices.size())).c_str());
    }

}

void Thing::draw()
{
    mat::translate(wellCenter[0]+0.5, 2, wellCenter[1]);

    if(false){
        program::bind(program::FIXED_COLOR);

        program::FixedColor* p2 = static_cast<program::FixedColor*>(program::getBound());

        glEnableVertexAttribArray(p2->vertHandle);

        shs->pos.x = 0; shs->pos.y = 0; shs->pos.z = lightPos.z;
        mat::scale(0.1,0.1,0.1);
        shs->draw();
        mat::scale(10,10,10);
        shs->pos.z = 0;
    }

    static float angle = 0;

    mat::rotate(angle,0,1,0);
    mat::rotate(fabs(angle-(2*M_PI)),1,0,0);

    if (!frozen)
    {
        angle += 0.005;
        if(angle>=(2*M_PI))
            angle = 0;
    }

    if (shs)
    {
        program::bind(program::FIXED_COLOR);

        program::FixedColor* p2 = static_cast<program::FixedColor*>(program::getBound());

        glEnableVertexAttribArray(p2->vertHandle);

        mat::downloadMVP();

        shs->drawMode = GL_LINE_LOOP;

        shs->draw();

    }

    if (true)
    {
        program::bind(program::LIGHTEN_TEXTURED_FIXED_COLOR);

        mat::downloadMVP();

        program::LightenTexturedFixedColor* p = static_cast<program::LightenTexturedFixedColor*>(program::getBound());

        glEnableVertexAttribArray(p->vertHandle);
        glEnableVertexAttribArray(p->texCoordHandle);
        glEnableVertexAttribArray(p->normalHandle);

        glUniform1i(p->doTexOffsetHandle,1);
        glUniform2f(p->texCoordOffsetHandle,GameScreen::instance->offsetX,GameScreen::instance->offsetY);

        auto vec = (mat::view*mat::model) * lightPos;

        glUniform3f(p->lightPosUniform,vec.x,vec.y,vec.z);
        glUniform1i(p->doLightUniform,1);
        glUniform1i(p->doLight2Uniform,0);
        glUniform1f(p->shininessUniform, 0.);
        glUniform1i(p->doUniformNormalsUniform, 0);

        for(int i=0;i<scene.size();i++){

            scene[i]->drawMode = GL_TRIANGLES;
            scene[i]->colors[0] = ColorRGB(0.75,0.75,0.75);
            scene[i]->draw();

            //SDL_Log(("Tex Coords size: "+std::to_string(scene[i]->texCoords.size())).c_str());

#if 0
            scene[i]->drawMode = GL_LINE_LOOP;
            scene[i]->colors[0] = ColorRGB(1,0.5,1);
            scene[i]->draw();
#endif
        }
    }
}
