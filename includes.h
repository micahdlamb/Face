//Include everything in a valid order, less efficient compiling but sick of dealing with dependency loops
#include <omp.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <cfloat>
#include <GL/glew.h>
#include <GL/freeglut.h>
//#define GLM_SWIZZLE convenient but makes debugger fucked
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/perpendicular.hpp>
#include <glm/gtc/noise.hpp>
#include <ctime>
#include <vector>
#include <map>
#include <stack>
#include <set>
#include "IL/il.h"
#include "IL/ilu.h"
#include "IL/ilut.h"

using namespace glm;
using namespace std;

struct Shader;
struct VAO;
struct Uniform;
struct ShapeBuilder;
struct BoundingSphere;
struct BoundingAABB;
struct Texture;

#include "Clock.h"
#include "Geometry.h"
#include "helpers.h"
#include "Viewport.h"
#include "VAO.h"
#include "Shapes.h"
#include "Shader.h"
#include "Texture.h"
#include "Framebuffer.h"
#include "Uniform.h"
#include "Viewer.h"
#include "Blurrer.h"
#include "Light.h"
#include "Scene.h"
#include "CubeMap.h"
#include "InputHandler.h"
#include "Background.h"
#include "Object.h"
#include "TexturedSquare.h"
#include "Perlin.h"