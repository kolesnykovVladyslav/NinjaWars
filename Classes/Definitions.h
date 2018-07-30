/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __DEFINITIONS_H__
#define __DEFINITIONS_H__

#define DISPLAY_TIME_SPLASH_SCENE 2
#define TRASNSITION_TIME 0.5
#define END_GAME_DELAY 3

enum LEVEL {FOREST, DESERT, CEMETERY};
enum GAME_RESULT {VICTORY, DEFEAT};
enum NINJA_CONTROL {PLAYER, BOT};

#define GRAVITY_FORCE 350

//NINJA
#define NINJA_FRAMES_COUNT 10
#define NINJA_SIZE 128
#define NINJA_MASS 128
#define NINJA_VELOCITY 100
#define NINJA_MAX_HP 1000
#define NINJA_JUMP_FORCE 35000
#define NINJA_COLLISION_BITMASK 0x01

//KUNAI
#define KUNAI_SIZE 40
#define KUNAI_MASS 1
#define KUNAI_COLLISION_BITMASK 0x02

//Aim
#define AIM_ANGLE_SPEED 100
#define AIM_RADIUS 100
#define MIN_AIM_ANGLE 0
#define MAX_AIM_ANGLE 90
#define MAX_THROW_POWER 1000
#define ANGULAR_VELOCITY 10

//game
#define GAME_TIME 90
#define BORDERS_COLLISION_BITMASK 0x03

#endif // __DEFINITIONS_H__
