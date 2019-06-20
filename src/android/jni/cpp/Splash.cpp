
#include "shapes.h"
#include "opengl/programs.h"
#include "Surface.h"

using namespace std;

static TexturedShape *spash;

void genSplash()
{
	spash = new TexturedShape();
	spash->verts = vector<Vec>(4);
	spash->verts[0] = Vec{-8,-1,-1};
	spash->verts[1] = Vec{8,-1,-8};
	spash->verts[2] = Vec{8,7,-8};
	spash->verts[3] = Vec{-8,7,-1};

	spash->indices = vector<unsigned short>(4);
	spash->indices[0] = 3;
	spash->indices[1] = 0;
	spash->indices[2] = 2;
	spash->indices[3] = 1;

	spash->texCoords = vector<Vec2>(4);
	spash->texCoords[0] = {0,1};
	spash->texCoords[1] = {1,1};
	spash->texCoords[2] = {1,0};
	spash->texCoords[3] = {0,0};

	spash->drawMode = GL_TRIANGLE_STRIP;
	spash->surface = new Surface("ggj-splash.png");
}

void drawSplash()
{
	program::bind(program::TEXTURED_FIXED_COLOR);

	program::TexturedFixedColor* p = static_cast<program::TexturedFixedColor*>(program::getBound());

	glEnableVertexAttribArray(p->vertHandle);
	glEnableVertexAttribArray(p->texCoordHandle);
	glUniform1i(p->doTexOffsetHandle,0);
	spash->draw();
}

