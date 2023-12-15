//
//  Level2.cpp
//  CS 3113 Project 5
//
//  Created by Jonathan Kim on 2023/12/06.
//

#include "Level2.hpp"
#include "Utility.hpp"

#define LEVEL_WIDTH 45
#define LEVEL_HEIGHT 8

unsigned int LEVEL2_DATA[] =
{
    55,  55,  55,  55,  55,  55,  55,  55,  55,  55,
    55,  55,  55,  55,  55,  55,  55,  55,  55,  55,
    55,  55,  55,  55,  55,  55,  55,  55,  55,  55,
    55,  55,  55,  55,  55,  55,  55,  55,  55,  55,
    55,  55,  55,  55,  55,
   
     0,   0,   0,   0,   0,   0,  55,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,  55,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
     0,  55,   0,   0,   0,
   
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
     0,  55,   0,   0,   0,
   
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,  55,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,
   
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,  55,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,  55,   0,
   
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,  55,   0,   0,
     0,  55,   0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,
   
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,  55,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,
   
    55,  55,  55,  55,  55,  55,  55,  55,  55,  55,
    55,  55,  55,  55,  55,  55,  55,  55,  55,  55,
    55,  55,  55,  55,  55,  55,  55,  55,  55,  55,
    55,  55,  55,  55,  55,  55,  55,  55,  55,  55,
    55,  55,  55,  55,  55,
};

GLuint level2_text_texture_id;

Level2::~Level2()
{
    delete [] m_state.enemies;
    delete    m_state.player;
    // delete    m_state.bullet;
    delete    m_state.map;
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeMusic(m_state.bgm);
}

void Level2::initialise()
{
    m_state.next_scene_id = -1;
    
    GLuint map_texture_id = Utility::load_texture("assets/images/tile_spritesheet.png");
    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL2_DATA, map_texture_id, 1.0f, 12, 13);
    
    // Code from main.cpp's initialise()
    /**
     George's Stuff
     */
    // Existing
    m_state.player = new Entity();
    m_state.player->set_entity_type(PLAYER);
    m_state.player->set_position(glm::vec3(1.0f, -3.0f, 0.0f));
    m_state.player->set_movement(glm::vec3(1.0f, 0.0f, 0.0f));
    m_state.player->set_speed(3.0f);
    m_state.player->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    m_state.player->set_scale(glm::vec3(1.0f, 1.0f, 1.0f));
    m_state.player->m_texture_id = Utility::load_texture("assets/images/player6.png");
    m_state.player->stage_clear = false;
    m_state.player->death = false;
    set_dead(false);
    
    // Walking
    m_state.player->set_height(0.9f);
    m_state.player->set_width(0.9f);
    
    // Jumping
    // m_state.player->m_jumping_power = 5.0f;
    
    /**
     Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture("assets/images/player3.png");
    
    m_state.enemies = new Entity[ENEMY_COUNT];
    m_state.enemies[0].set_entity_type(ENEMY);
    m_state.enemies[0].set_ai_type(GUARD);
    m_state.enemies[0].set_ai_state(IDLE);
    m_state.enemies[0].m_texture_id = enemy_texture_id;
    m_state.enemies[0].set_position(glm::vec3(15.0f, -3.0f, 0.0f));
    m_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_state.enemies[0].set_speed(1.0f);
    m_state.enemies[0].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    m_state.enemies[0].set_scale(glm::vec3(1.0f, 1.0f, 1.0f));
    
//    m_state.bullet = new Entity();
//    m_state.bullet->set_entity_type(BULLET);
//    m_state.bullet->set_position(m_state.player->get_position());
//    m_state.bullet->set_movement(glm::vec3(1.0f, 0.0f, 0.0f));
//    m_state.bullet->set_speed(4.0f);
//    m_state.bullet->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
//    m_state.bullet->bullet_deactivate();
//    m_state.bullet->set_scale(glm::vec3(0.5f, 0.5f, 0.5f));
//    m_state.bullet->m_texture_id = Utility::load_texture("assets/images/bullet.png");
//    
//    m_state.bullet->set_height(0.5f);
//    m_state.bullet->set_width(0.5f);
    
    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_state.bgm = Mix_LoadMUS("assets/audio/bgm.mp3");
    Mix_PlayMusic(m_state.bgm, -1);
    Mix_VolumeMusic(50.0f);
    
    m_state.jump_sfx = Mix_LoadWAV("assets/audio/stage_clear.wav");
    
    level2_text_texture_id = Utility::load_texture("assets/fonts/font1.png");
}

void Level2::update(float delta_time)
{
    m_state.player->update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map);

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_state.enemies[i].update(delta_time, m_state.player, 0, 0, m_state.map);
    }


    
    if (m_state.player->get_position().x > 55.0f) {
        Mix_PlayChannel(-1, m_state.jump_sfx, 0);
        m_state.next_scene_id = 3;
    }
    if (m_state.player->death) {
        m_state.next_scene_id = 0;
    }
}


void Level2::render(ShaderProgram *program)
{
    m_state.map->render(program);
    m_state.player->render(program);
    // m_state.bullet->render(program);
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        if (m_state.enemies[i].get_active()) {
            m_state.enemies[i].render(program);
        }
    }
}
