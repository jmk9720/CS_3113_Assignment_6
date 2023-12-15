//
//  End_Screen.cpp
//  CS 3113 Project 6
//
//  Created by Jonathan Kim on 12/15/23.
//

#include "End_Screen.hpp"
#include "Utility.hpp"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

unsigned int END_SCREEN_DATA[] =
{
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0, 103, 103, 103, 103, 103, 103, 103, 103, 103,   0,   0,   0,   0,
    0, 103,   0,   0,   0,   0,   0,   0,   0, 103,   0,   0,   0,   0,
    0, 103,   0,   0,   0,   0,   0,   0,   0, 103,   0,   0,   0,   0,
    0, 103,   0,   0,   0,   0,   0,   0,   0, 103,   0,   0,   0,   0,
    0, 103,   0,   0,   0,   0,   0,   0,   0, 103,   0,   0,   0,   0,
    0, 103, 103, 103, 103, 103, 103, 103, 103, 103,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};
GLuint end_screen_text_texture_id;

End_Screen::~End_Screen()
{
    delete    m_state.map;
    Mix_FreeMusic(m_state.bgm);
}

void End_Screen::initialise()
{
    m_state.next_scene_id = -1;
    
    GLuint map_texture_id = Utility::load_texture("assets/images/tile_spritesheet.png");
    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, END_SCREEN_DATA, map_texture_id, 1.0f, 12, 13);
    
    
    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_state.bgm = Mix_LoadMUS("assets/audio/bgm.mp3");
    Mix_PlayMusic(m_state.bgm, -1);
    Mix_VolumeMusic(50.0f);
    
    end_screen_text_texture_id = Utility::load_texture("assets/fonts/font1.png");
}

void End_Screen::update(float delta_time)
{
    
}


void End_Screen::render(ShaderProgram *program)
{
    m_state.map->render(program);
    Utility::draw_text(program, end_screen_text_texture_id, "F@&* YOU!", 0.5f, 0.0f, glm::vec3(3.0f, -3.0f, 0.0f));
}
