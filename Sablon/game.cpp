#include "game.h"
#include <algorithm>

#include "resource_manager.h"
#include "sprite_renderer.h"
#include "model_renderer.h"
#include "game_object.h"
#include <vector>
#include <cstdlib> 
#include <ctime>
#include <thread>

#include "light_renderer.h"
#include "text_renderer.h"
#include "model.h"

using namespace std;

SpriteRenderer* Renderer;
GameObject* Player;
GameObject* Sun;
GameObject* Moon;
GameObject* Desert;
GameObject* Sky;
GameObject* Star;
vector<GameObject*> Stars;
vector<GameObject*> Grass;
GameObject* Grass3D;
vector<GameObject*> Pyramids;
vector<GameObject*> Doors;
GameObject* Water;
GameObject* Fish;
TextRenderer* Text;
glm::mat4 projection;
glm::mat4 view;
ModelRenderer* sky_box;
Model pyramid;
Model desert;
Model sky_box_model;
Model FishModel;
Model sun_model;
Model moon_model;
Model pyramid_hallway_model;
ModelRenderer* fish_model_renderer;
ModelRenderer* pyramid_renderer1;
ModelRenderer* pyramid_renderer2;
ModelRenderer* pyramid_renderer3;
ModelRenderer* pyramid_hallway_renderer1;
ModelRenderer* pyramid_hallway_renderer2;
ModelRenderer* pyramid_hallway_renderer3;
ModelRenderer* moon_renderer;
ModelRenderer* sun_renderer;
Model Light;
LightRenderer* light_renderer1;
LightRenderer* light_renderer2;
LightRenderer* light_renderer3;
LightRenderer* light_renderer4;

glm::mat4 orthogonal_projection;
glm::mat4 perspective_projection;
bool is_perspective_projection = true;
float GrassRotation = 0.0f;
int moveFish = 300;
bool is_phong_lighting = true;


Game::Game(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{
    
}

Game::Game()
{
}

Game::~Game()
{
    delete Renderer;
    delete Player;
    delete Sun;
    delete Moon;
    delete Desert;
    delete Sky;
    delete Water;
    delete Fish;

    for (const auto& star : Stars) {
        delete star;
    }
    for (const auto& grass : Grass) {
        delete grass;
    }
    for (const auto& pyramid: Pyramids) {
        delete pyramid;
    }
    for (const auto& door: Doors) {
        delete door;
    }
}

void Game::Init()
{
	// load shaders
    ResourceManager::LoadShader("model.vs", "model.fs", nullptr, "model");

    ResourceManager::LoadShader("light.vs", "light.fs", nullptr, "light");
    orthogonal_projection = glm::ortho(0.0f, static_cast<float>(this->Width)/2,
        static_cast<float>(this->Height)/2, 0.0f, -1000.0f, 4000.0f);
    perspective_projection = glm::perspective(glm::radians(45.0f), (float)Width /
        (float)Height, 0.1f, 5000.0f);
	// configure shaders
    glm::mat4 projection = perspective_projection;
    ResourceManager::GetShader("light").Use().SetMatrix4("projection", projection);

    ResourceManager::GetShader("model").Use().SetInteger("image", 0);
    ResourceManager::GetShader("model").SetMatrix4("projection", projection);
    ResourceManager::GetShader("model_gouard").Use().SetInteger("image", 0);
    ResourceManager::GetShader("model_gouard").SetMatrix4("projection", projection);
    ResourceManager::LoadShader("sprite.vert", "sprite.frag", nullptr, "sprite");
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

    // set render-specific controls
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	// load textures
	ResourceManager::LoadTexture("res/texel_checker.png", false, "face");
	ResourceManager::LoadTexture("res/sun.png", true, "sun");
	ResourceManager::LoadTexture("res/moon.png", true, "moon");
	ResourceManager::LoadTexture("res/desert.png", true, "desert");
	ResourceManager::LoadTexture("res/sky.png", true, "sky");
	ResourceManager::LoadTexture("res/star.png", true, "star");
	ResourceManager::LoadTexture("res/water_shaped.png", true, "water");
	ResourceManager::LoadTexture("res/fish.png", true, "fish");
	ResourceManager::LoadTexture("res/grass.png", true, "grass");
	ResourceManager::LoadTexture("res/pyramid.png", true, "pyramid");
	ResourceManager::LoadTexture("res/door.jpg", true, "door");

	Sun = new GameObject(glm::vec3(this->Width - 200.0f, this->Height / 2.0f - 100.0f, 0.0f), glm::vec2(200.0f, 200.0f),
	                     ResourceManager::GetTexture("sun"));
	Moon = new GameObject(glm::vec3(0.0f, this->Height / 2.0f - 100.0f, 0.0f), glm::vec2(200.0f, 200.0f),
	                      ResourceManager::GetTexture("moon"));
	Desert = new GameObject(glm::vec3(0.0f, Height / 2, 0.0f), glm::vec2(Width, Height / 2),
	                        ResourceManager::GetTexture("desert"));
	Sky = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(Width, Height), ResourceManager::GetTexture("sky"));
	Water = new GameObject(glm::vec3(-600.0f, -500.0f, -600.0f), glm::vec2(1500.0f, 1000.0f),
	                       ResourceManager::GetTexture("water"), glm::vec3(1.0f), glm::vec2(0.0f, 0.0f), 0.7f);
    Water->Rotation.x = 90.0f;

	Grass3D = new GameObject(glm::vec3(-600.0f, 0.0f, -600.0f), glm::vec2(200.0f, 150.0f),
	                         ResourceManager::GetTexture("grass"));
    Grass3D->Rotation = glm::vec3(0.0f, 0.0f, 180.0f);

	_initializeStars();
	_initializePyramids();
    _initializeDoors();
	Fish = new GameObject(glm::vec3(Width / 1.45f, Height / 1.1f, 0.0f), glm::vec2(Width / 30, Width / 30),
	                      ResourceManager::GetTexture("fish"));
    Text = new TextRenderer(Width, Height);
    Text->Load("fonts/Antonio-Regular.ttf", 24);
    
    sky_box = new ModelRenderer(ResourceManager::GetShader("model"), 
        glm::vec3(-750.0f, -100.0f, -750.0f), 
        glm::vec3(1500.0f, 1500.0f, 1500.0f),
        glm::vec3(90.0f, 0.0f, 0.0f));
    pyramid = Model("res/backpack/pyramid/pyramid_spec2.obj");
    desert = Model("res/backpack/sand/desert.obj");
    sky_box_model = Model("res/backpack/skybox.obj");
	FishModel = Model("res/backpack/clownfish.obj");
    Light = Model("res/backpack/transparent_cube.obj");
    sun_model = Model("res/backpack/sun/sun.obj");
    moon_model = Model("res/backpack/moon/moon.obj");
    pyramid_hallway_model = Model("res/backpack/pyramid/door_hallway.obj");
    
    fish_model_renderer = new ModelRenderer(ResourceManager::GetShader("model"),
        glm::vec3(-200.0f, 0.0f, -700.0f),
        glm::vec3(3.0f, 3.0f, 3.0f),
        glm::vec3(90.0f, 0.0f, 0.0f));
    light_renderer1 = new LightRenderer(ResourceManager::GetShader("light"),
        glm::vec3(600.0f, 450.0f, -600.0f),
        glm::vec3(2.0f, 2.0f, 2.0f),
        glm::vec3(0.0f, 0.0f, 0.0f));
    light_renderer2 = new LightRenderer(ResourceManager::GetShader("light"),
        glm::vec3(500.0f, 330.0f, 100.0f),
        glm::vec3(2.0f, 2.0f, 2.0f),
        glm::vec3(0.0f, 0.0f, 0.0f));
    light_renderer3 = new LightRenderer(ResourceManager::GetShader("light"),
        glm::vec3(-200.0f, 250.0f, 300.0f),
        glm::vec3(2.0f, 2.0f, 2.0f),
        glm::vec3(0.0f, 0.0f, 0.0f));
    light_renderer4 = new LightRenderer(ResourceManager::GetShader("light"),
        glm::vec3(600.0f, 430.0f, -600.0f),
        glm::vec3(20.0f, 20.0f, 20.0f),
        glm::vec3(0.0f, 0.0f, 0.0f));
    pyramid_renderer1 = new ModelRenderer(ResourceManager::GetShader("model"),
        glm::vec3(600.0f, -100.0f, -600.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(0.0f, 0.0f, 0.0f));
    pyramid_hallway_renderer1 = new ModelRenderer(ResourceManager::GetShader("model"),
        glm::vec3(600.0f, 50.0f, -900.0f),
        glm::vec3(50.0f, 50.0f, 50.0f),
        glm::vec3(0.0f, 0.0f, 0.0f));
    pyramid_renderer2 = new ModelRenderer(ResourceManager::GetShader("model"),
        glm::vec3(500.0f, -50.0f, 100.0f),
        glm::vec3(200.0f, 200.0f, 200.0f),
        glm::vec3(0.0f, 0.0f, 0.0f));
    pyramid_hallway_renderer2 = new ModelRenderer(ResourceManager::GetShader("model"),
        glm::vec3(500.0f, 100.0f, -50.0f),
        glm::vec3(50.0f, 50.0f, 50.0f),
        glm::vec3(0.0f, 0.0f, 0.0f));
    pyramid_renderer3 = new ModelRenderer(ResourceManager::GetShader("model"),
        glm::vec3(-200.0f, -40.0f, 300.0f),
        glm::vec3(150.0f, 150.0f, 150.0f),
        glm::vec3(0.0f, 0.0f, 0.0f));
    pyramid_hallway_renderer3 = new ModelRenderer(ResourceManager::GetShader("model"),
        glm::vec3(-200.0f, 100.0f, 200.0f),
        glm::vec3(50.0f, 50.0f, 50.0f),
        glm::vec3(0.0f, 0.0f, 0.0f));
    moon_renderer = new ModelRenderer(ResourceManager::GetShader("model"),
        glm::vec3(-1500.0f, 0.0f, 0.0f),
        glm::vec3(100.0f, 100.0f, 100.0f),
        glm::vec3(0.0f, 0.0f, 0.0f));
    sun_renderer = new ModelRenderer(ResourceManager::GetShader("model"),
        glm::vec3(1500.0f, 0.0f, 0.0f),
        glm::vec3(10.0f, 10.0f, 10.0f),
        glm::vec3(0.0f, 0.0f, 0.0f));
}


void Game::Update(float dt)
{
    _updateSunAndMoon(dt);
    _updateSkyBrightness(dt);
    _moveFish(dt);
    if(_startOpeningDoors)
    {
        _openDoors(dt);
    }
    view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.0f));
    view = glm::rotate(view, glm::radians(Renderer->CameraAngleX), glm::vec3(1.0f, 0.0f, 0.0f));
    view = glm::rotate(view, glm::radians(Renderer->CameraAngleY), glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::translate(view, glm::vec3(Renderer->CameraPositionX, Renderer->CameraPositionY, Renderer->CameraPositionZ));
    ResourceManager::GetShader("light").Use().SetMatrix4("view", view);
	ResourceManager::GetShader("model").Use().SetMatrix4("view", view);
    ResourceManager::GetShader("model").Use().SetVector3f("viewPos", Renderer->CameraPositionX, Renderer->CameraPositionY, Renderer->CameraPositionZ);
    glm::mat4 model3D = glm::mat4(1.0f);
    model3D = glm::translate(model3D, glm::vec3(-750.0f, -100.0f, -750.0f)); // translate it down so it's at the center of the scene
    model3D = glm::scale(model3D, glm::vec3(1500.0f, 1500.0f, 1500.0f));	// it's a bit too big for our scene, so scale it down
    model3D = glm::rotate(model3D, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    ResourceManager::GetShader("model").SetMatrix4("model", model3D);
    

    ResourceManager::GetShader("sprite").Use();
}

void Game::ProcessInput(int key)
{
    if (key == GLFW_KEY_R) {
        _sunAngle = 180.0f;
        _timeSpeed = 50.0f;
    }
    if (key == GLFW_KEY_P)
    {
        _timeSpeed = 0.0f;
    }
    if (key == GLFW_KEY_S)
    {
        _initializeStars();
    }
    if (key == GLFW_KEY_F)
    {
        Fish->FlipHorizontally();
    }
    if (key == GLFW_KEY_3)
    {
        _initializePyramids();
    }
    if (key == GLFW_KEY_O)
    {
        _toggleDoorVisibility();
    }
	if (key == GLFW_KEY_G)
    {
        _initializeGrass();
    }
    if (key == GLFW_KEY_1 || key == GLFW_KEY_2)
    {
	    _toggleGrassVisibility();
    }
    if (key == GLFW_KEY_Z)
    {
        _toggleProjection();
    }
    if (key == GLFW_KEY_L)
    {
        _toggleLightModel();
    }

    if (Keys[GLFW_KEY_D])
    {
	    GameObject* largestPyramid = GetLargestPyramid();
        largestPyramid->Threshold += 0.01f;
        if (largestPyramid->Threshold > 1.0f) {
            largestPyramid->Threshold = 1.0f;
        }
    }
    if (Keys[GLFW_KEY_A])
    {
        GameObject* largestPyramid = GetLargestPyramid();
        largestPyramid->Threshold -= 0.01f;
        if (largestPyramid->Threshold < 0.0f) {
            largestPyramid->Threshold = 0.0f;
        }
    }
    if (Keys[GLFW_KEY_UP]) {
        Renderer->CameraAngleX -= 2.0f;
    }
    if (Keys[GLFW_KEY_DOWN]) {
        Renderer->CameraAngleX += 2.0f;
    }
    if (Keys[GLFW_KEY_LEFT]) {
        Renderer->CameraAngleY -= 2.0f;
    }
    if (Keys[GLFW_KEY_RIGHT]) {
        Renderer->CameraAngleY += 2.0f;
    }

    if (Keys[GLFW_KEY_PAGE_UP]) {
        Renderer->CameraPositionX += 2.0f;
    }
    if (Keys[GLFW_KEY_PAGE_DOWN]) {
        Renderer->CameraPositionX -= 2.0f;
    }
    if (Keys[GLFW_KEY_NUM_LOCK]) {
        Renderer->CameraPositionY += 2.0f;
    }
    if (Keys[GLFW_KEY_HOME]) {
        Renderer->CameraPositionY -= 2.0f;
    }
    if (Keys[GLFW_KEY_KP_SUBTRACT]) {
        Renderer->CameraPositionZ -= 2.0f;
    }
    if (Keys[GLFW_KEY_KP_MULTIPLY]) {
        Renderer->CameraPositionZ += 2.0f;
    }
}

void Game::ProcessMouseClick(double x, double y)
{
    for (const auto& door : Doors)
    {
	    if(door->Alpha == 1.0f && door->Position.x <= x && door->Position.x + door->Size.x >= x && door->Position.y <= y && door->Position.y + door->Size.y >= y)
	    {
		    _isDisplayedToBeContinued = true;
            break;
	    }
    }
}

bool Game::Render()
{
    ResourceManager::GetShader("sprite").Use();
    //Sky->Draw(*Renderer);
    for (const auto& star : Stars)
    {
    //    star->Draw(*Renderer);
    }

    //Sun->Draw(*Renderer);
    //Moon->Draw(*Renderer);
    //Desert->Draw(*Renderer);
    
    //Fish->Draw(*Renderer);
    for (const auto& grass: Grass)
    {
        //grass->Draw(*Renderer);
    }
    Text->RenderText("Ognjen Gligoric SV79/2021", Width/30, Height/30, 1.0f);
    
    if (_isDisplayedToBeContinued)
    {
	    Text->RenderText("To be continued in 3D game", Width / 2, Height / 4, 3.0f,glm::vec3(1),1.0f, _toBeContinuedThreshold);
        _toBeContinuedThreshold += 0.01;
        
        if (_toBeContinuedThreshold >= 0.99f && _toBeContinuedThreshold <= 2.0f)
        {
            _shouldClose = true;
            _toBeContinuedThreshold = 2.1f;
        }
    }
    if (_shouldClose)
    {
        _shouldClose = false;
        return true;
    }
    //sky_box->DrawModel(sky_box_model);
    pyramid_renderer1->DrawModel(pyramid);
    pyramid_renderer2->DrawModel(pyramid);
    pyramid_renderer3->DrawModel(pyramid);
    pyramid_hallway_renderer1->DrawModel(pyramid_hallway_model);
    pyramid_hallway_renderer2->DrawModel(pyramid_hallway_model);
    pyramid_hallway_renderer3->DrawModel(pyramid_hallway_model);
    sun_renderer->DrawModel(sun_model);
    moon_renderer->DrawModel(moon_model);
    glm::mat4 desertModel = glm::mat4(1.0f);
    desertModel = glm::translate(desertModel, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    desertModel = glm::scale(desertModel, glm::vec3(100.0f, 100.0f, 100.0f));
    ResourceManager::GetShader("model").Use().SetMatrix4("model", desertModel);
    desert.Draw(ResourceManager::GetShader("model").Use());
    fish_model_renderer->DrawModel(FishModel);
    light_renderer1->DrawLight(Light);
    light_renderer2->DrawLight(Light);
    light_renderer3->DrawLight(Light);

    glDepthMask(GL_FALSE);
    //Water->Draw(*Renderer);
    Grass3D->Draw(*Renderer);
    Grass3D->Rotation.y = 90.0f;
    Grass3D->Draw(*Renderer);
    Grass3D->Rotation.y = 0.0f;
    for (size_t i = 0; i < Pyramids.size(); ++i)
    {
        Doors[i]->Draw(*Renderer);
    }
    glDepthMask(GL_TRUE);
    return false;
}


void Game::_updateSunAndMoon(float dt)
{
    const auto circleCenter = glm::vec3(0.0f, 0.0f, 0.0f);

    _sunAngle += _timeSpeed * dt;       

    if (_sunAngle > 360.0f)
        _sunAngle -= 360.0f;

    float sunRadians = glm::radians(_sunAngle);
    float moonRadians = sunRadians + glm::pi<float>(); 

    sun_renderer->Position.x = 1500.0f * cos(sunRadians);
    sun_renderer->Position.y = 1500.0f * sin(sunRadians);

    moon_renderer->Position.x = 1500.0f * cos(moonRadians);
    moon_renderer->Position.y = 1500.0f * sin(moonRadians);
}

void Game::_updateSkyBrightness(float dt) const
{
    float normalizedHeight = (_getSunRiseHeightPoint() - Sun->Position.y) / _getSunRotationRadius();
    normalizedHeight = glm::clamp(normalizedHeight, 0.0f, 1.0f);

    const glm::vec3 darkestColor = glm::vec3(0.0f, 0.0f, 0.3f); // Midnight blue
    const glm::vec3 brightestColor = glm::vec3(0.5f, 0.7f, 1.0f); // Sky blue

    const glm::vec3 currentColor = glm::mix(darkestColor, brightestColor, normalizedHeight);

    Sky->Color = currentColor;
    const float starVisibility = 1.0f - normalizedHeight; 
    for (const auto& star : Stars) {
        star->Alpha = starVisibility;
    }
}

void Game::_initializeStars() const
{
    srand(static_cast<unsigned>(std::time(nullptr)));
    constexpr int starCount = 50;

    while (Stars.size() < starCount) {
        Stars.push_back(new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), ResourceManager::GetTexture("star")));
    }

    for (int i = 0; i < starCount; ++i) {
        const auto x = static_cast<float>(rand() % Width);
        const auto y = static_cast<float>(rand() % static_cast<int>(_getSunRiseHeightPoint()));
        const auto size = static_cast<float>(10 + rand() % 21);
        Stars[i]->Position = glm::vec3(x, y, 0.0f);
        Stars[i]->Size = glm::vec2(size, size);
        Stars[i]->Alpha = 1.0f; 
    }
}

void Game::_initializePyramids()
{
    srand(static_cast<unsigned>(std::time(nullptr)));
    constexpr int pyramidCount = 3;

    while (Pyramids.size() < pyramidCount) {
        Pyramids.push_back(new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), ResourceManager::GetTexture("pyramid")));
    }

    for (int i = 0; i < pyramidCount; ++i) {
        const auto size = static_cast<float>(Width) / 10 + rand() % 100;
        const auto x = static_cast<float>(rand() % Width/2);
        const auto y = _getSunRiseHeightPoint() - size + rand() % static_cast<int>(Height - _getSunRiseHeightPoint() - size);
        Pyramids[i]->Position = glm::vec3(x, y, 0.0f);
        Pyramids[i]->Size = glm::vec2(size, size);
        Pyramids[i]->Alpha = 1.0f;
        Pyramids[i]->Threshold = 0.0f;
    }

    std::sort(Pyramids.begin(), Pyramids.end(), [](const GameObject* a, const GameObject* b)
    {
        return a->Position.y + a->Size.y < b->Position.y + b->Size.y;
    });
}

void Game::_initializeGrass() const
{
    srand(static_cast<unsigned>(std::time(nullptr)));
    constexpr int grassCount = 30;

    while (Grass.size() < grassCount) {
        Grass.push_back(new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), ResourceManager::GetTexture("grass")));
    }

    for (int i = 0; i < grassCount; ++i) {
        const auto size = static_cast<float>(Width) / 15 + rand() % 200;
        const auto x = Water->Position.x - size/2 + rand() % static_cast<int>(Water->Size.x);
        const auto y = Water->Position.y + Water->Size.y - size +  (rand() % static_cast<int>(Width/30) - Width/60);
        Grass[i]->Position = glm::vec3(x, y, 0.0f);
        Grass[i]->Size = glm::vec2(size, size);
        Grass[i]->Alpha = 1.0f;
    }

    std::sort(Grass.begin(), Grass.end(), [](const GameObject* a, const GameObject* b)
        {
            return a->Position.y + a->Size.y < b->Position.y + b->Size.y;
        });
}

void Game::_moveFish(float dt)
{
    moveFish = (abs(moveFish) + 1) % 600;
    int direction = moveFish - 300;
    direction = (direction < 0) ? -1 : 1;
    fish_model_renderer->Position.z += static_cast<float>(direction);
    fish_model_renderer->Rotation.x = (direction < 0) ? -90.0f : 90.0f;
    fish_model_renderer->Rotation.y = (direction < 0) ? 180.0f : 0.0f;
}

void Game::_toggleGrassVisibility()
{
    for (const auto& grass : Grass)
    {
        grass->Alpha = static_cast<float>((static_cast<int>(grass->Alpha)+1) % 2);
    }
}

float Game::_getSunRiseHeightPoint() const
{
    return this->Height / 1.5f;
}

float Game::_getSunRotationRadius() const
{
    return this->Width / 3.0f;
}

auto Game::GetLargestPyramid() const -> GameObject*
{
    return *max_element(Pyramids.begin(), Pyramids.end(), [](const GameObject* a, const GameObject* b) {
        return (a->Size.x * a->Size.y) < (b->Size.x * b->Size.y); 
        });
}

void Game::_initializeDoors() const
{
    Doors.clear(); 
    Doors.shrink_to_fit();
    
    Doors.push_back(new GameObject(glm::vec3(550.0f, 0.0f, -952.0f), glm::vec2(100.0f, 100.0f), ResourceManager::GetTexture("door")));
    Doors.push_back(new GameObject(glm::vec3(450.0f, 50.0f, -102.0f), glm::vec2(100.0f, 100.0f), ResourceManager::GetTexture("door")));
    Doors.push_back(new GameObject(glm::vec3(-250.0f, 50.0f, 148.0f), glm::vec2(100.0f, 100.0f), ResourceManager::GetTexture("door")));

    for (const auto& door : Doors)
    {
        door->Alpha = 1.0f;
        door->Rotation = glm::vec3(0.0f, 0.0f, 90.0f);
        door->HighlightColor = glm::vec3(0.0f, 0.0f, 0.0f);
        door->Threshold = 0.0f;
    }
}

void Game::_toggleDoorVisibility()
{
    for (const auto& door : Doors)
    {
        _startOpeningDoors = true;
    }
}

void Game::_toggleLightModel()
{
	is_phong_lighting = !is_phong_lighting;
    if (is_phong_lighting) {
        ResourceManager::LoadShader("model.vs", "model.fs", nullptr, "model");
    }
    else {
        ResourceManager::LoadShader("model_gouard.vs", "model_gouard.fs", nullptr, "model");
    }
    ResourceManager::GetShader("model").Use().SetInteger("image", 0);
    if (is_perspective_projection) {
        ResourceManager::GetShader("model").SetMatrix4("projection", perspective_projection);
    }
    else {
        ResourceManager::GetShader("model").SetMatrix4("projection", orthogonal_projection);
    }
    fish_model_renderer->shader = ResourceManager::GetShader("model");

    pyramid_renderer1->shader = ResourceManager::GetShader("model");
    pyramid_hallway_renderer1->shader = ResourceManager::GetShader("model");
    pyramid_renderer2->shader = ResourceManager::GetShader("model");
    pyramid_hallway_renderer2->shader = ResourceManager::GetShader("model");
    pyramid_renderer3->shader = ResourceManager::GetShader("model");
    pyramid_hallway_renderer3->shader = ResourceManager::GetShader("model");
    moon_renderer->shader = ResourceManager::GetShader("model");
    sun_renderer->shader = ResourceManager::GetShader("model");
}

auto Game::_openDoors(float dt) -> void
{
    for (const auto& door : Doors)
    {
        door->Threshold += 0.01f;
        if (door->Threshold > 1.0f) {
            door->Threshold = 1.0f; 
        }
    }
}


void Game::_toggleProjection()
{
    is_perspective_projection = !is_perspective_projection;
    if(is_perspective_projection)
    {
        for (const auto pair : ResourceManager::Shaders) {
            ResourceManager::GetShader(pair.first).Use().SetMatrix4("projection", perspective_projection);
        }
    }else
    {
        for (const auto pair : ResourceManager::Shaders) {
            ResourceManager::GetShader(pair.first).Use().SetMatrix4("projection", orthogonal_projection);
        }
    }
}