/**
* Author: Jonathan Kim
* Assignment: Final Project
* Date due: 2023-12-15, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LEVEL1_LEFT_EDGE 45.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.hpp"
#include "Map.hpp"
#include "Utility.hpp"
#include "Scene.hpp"
#include "Level1.hpp"
#include "Level2.hpp"
#include "Level3.hpp"
#include "Start_Screen.hpp"
#include "End_Screen.hpp"

// ————— CONSTANTS ————— //
const int   WINDOW_WIDTH    = 640,
            WINDOW_HEIGHT   = 480;

const float BG_RED      = 0.1922f,
            BG_BLUE     = 0.549f,
            BG_GREEN    = 0.9059f,
            BG_OPACITY  = 1.0f;

const int   VIEWPORT_X = 0,
            VIEWPORT_Y = 0,
            VIEWPORT_WIDTH = WINDOW_WIDTH,
            VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char  V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
            F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const float MILLISECONDS_IN_SECOND = 1000.0;

const int FONTBANK_SIZE = 16;

const char TEXT_SPRITE_FILEPATH[] = "assets/fonts/font1.png";

// GLuint text_texture_id;

// ————— GLOBAL VARIABLES ————— //
Scene*  g_current_scene;
Start_Screen* g_start_screen;
Level1* g_level_1;
Level2* g_level_2;
Level3* g_level_3;
End_Screen* g_end_screen;

Scene *g_levels[5];

SDL_Window* g_display_window;
bool g_game_is_running = true;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks  = 0.0f;
float g_accumulator     = 0.0f;

int life_count = 0;
bool death = false;
// bool mission;

void switch_to_scene(Scene* scene)
{
    g_current_scene = scene;
    g_current_scene->initialise();
}

void initialise()
{
    // ————— VIDEO ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("FINAL PROJECT",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    // ————— GENERAL ————— //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.Load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.SetProjectionMatrix(g_projection_matrix);
    g_shader_program.SetViewMatrix(g_view_matrix);

    glUseProgram(g_shader_program.programID);

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    // ————— LEVEL A SETUP ————— //
    g_start_screen = new Start_Screen();
    g_level_1 = new Level1();
    g_level_2 = new Level2();
    g_level_3 = new Level3();
    g_end_screen = new End_Screen();
    
    g_levels[0] = g_start_screen;
    g_levels[1] = g_level_1;
    g_levels[2] = g_level_2;
    g_levels[3] = g_level_3;
    g_levels[4] = g_end_screen;
    switch_to_scene(g_levels[0]);
    
    //text_texture_id = Utility::load_texture(TEXT_SPRITE_FILEPATH);
    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    if (g_current_scene != g_start_screen && g_current_scene != g_end_screen) {
        g_current_scene->m_state.player->set_movement(glm::vec3(1.0, 0.0f, 0.0f));
    }

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        // ————— KEYSTROKES ————— //
        switch (event.type) {
            // ————— END GAME ————— //
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_game_is_running = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q:
                // Quit the game with a keystroke
                g_game_is_running = false;
                break;

            case SDLK_SPACE:
                if (g_current_scene != g_start_screen && !g_current_scene->m_state.bullet->get_active())
                {
                    g_current_scene->m_state.bullet->bullet_activate();
                    g_current_scene->m_state.bullet->set_position(g_current_scene->m_state.player->get_position());
                }
                break;
                    
            case SDLK_RETURN:
                if (g_current_scene == g_start_screen) {
                    g_current_scene->m_state.game_over = false;
                    g_current_scene->m_state.mission = false;
                    switch_to_scene(g_level_1);
                }
                else if (g_current_scene == g_end_screen) {
                    switch_to_scene(g_start_screen);
                }
                break;

            default:
                break;
            }

        default:
            break;
        }
    }

    // ————— KEY HOLD ————— //
    if (g_current_scene != g_start_screen && g_current_scene != g_end_screen) {
        const Uint8* key_state = SDL_GetKeyboardState(NULL);

        if (key_state[SDL_SCANCODE_UP])
        {
            g_current_scene->m_state.player->move_up();
    //        g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->LEFT];
        }
        else if (key_state[SDL_SCANCODE_DOWN])
        {
            g_current_scene->m_state.player->move_down();
    //        g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->RIGHT];
        }

        // ————— NORMALISATION ————— //
        if (glm::length(g_current_scene->m_state.player->get_movement()) > 1.0f)
        {
            g_current_scene->m_state.player->set_movement(glm::normalize(g_current_scene->m_state.player->get_movement()));
        }
    }
    
}

void update()
{
    // ————— DELTA TIME / FIXED TIME STEP CALCULATION ————— //
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    delta_time += g_accumulator;

    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }
    
    while (delta_time >= FIXED_TIMESTEP) {
        // ————— UPDATING THE SCENE (i.e. map, character, enemies...) ————— //
        g_current_scene->update(FIXED_TIMESTEP);
        
        delta_time -= FIXED_TIMESTEP;
    }
    g_accumulator = delta_time;
    
    
    // ————— PLAYER CAMERA ————— //
    g_view_matrix = glm::mat4(1.0f);
    if (g_current_scene == g_start_screen || g_current_scene == g_end_screen) {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 3.75, 0));
    }
    else if (g_current_scene->m_state.player->get_position().x > LEVEL1_LEFT_EDGE) {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-LEVEL1_LEFT_EDGE - 3.5f, 3.75, 0));
        std::cout << g_current_scene->m_state.player->get_position().x << std::endl;
    }
    else {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->m_state.player->get_position().x - 3.5f, 3.75, 0));
    }
    
//    if (g_current_scene != g_start_screen && g_current_scene->m_state.player->get_position().x > LEVEL1_LEFT_EDGE) {
//            g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->m_state.player->get_position().x, 3.75, 0));
//        } else {
//            g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 3.75, 0));
//        }
}

void render()
{
    g_shader_program.SetViewMatrix(g_view_matrix);

    glClear(GL_COLOR_BUFFER_BIT);

    // ————— RENDERING THE SCENE (i.e. map, character, enemies...) ————— //
    glUseProgram(g_shader_program.programID);
    g_current_scene->render(&g_shader_program);
    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();

    // ————— DELETING LEVEL A DATA (i.e. map, character, enemies...) ————— //
    delete g_start_screen;
    delete g_level_1;
    delete g_level_2;
    delete g_level_3;
    delete g_end_screen;
}

// ————— GAME LOOP ————— //
int main(int argc, char* argv[])
{
    initialise();

    while (g_game_is_running)
    {
        process_input();
        update();
        
        if (g_current_scene->m_state.next_scene_id >= 0) {
            switch_to_scene(g_levels[g_current_scene->m_state.next_scene_id]);
        }
        render();
    }

    shutdown();
    return 0;
}
