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

#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "Definitions.h"
#include "Ninja.h"

class GameScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene(LEVEL level);

    virtual bool init();
    
    // implement the "static create()" method manually
    CREATE_FUNC(GameScene);
    
    cocos2d::Vector<cocos2d::SpriteFrame*> getAnimation(const char *format, int count);
private:
    void setPhysicsWorld(cocos2d::PhysicsWorld *world) { sceneWorld = world;};
    cocos2d::PhysicsWorld *sceneWorld;
    cocos2d::Layer *collisionLayer;
    bool onContactBegin(cocos2d::PhysicsContact& contact);
    
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event * event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event * event);
    
    Ninja *playerNinja;
    std::vector<Ninja*> enemies;
    Ninja* respawnNinja(cocos2d::Value object, NINJA_CONTROL ninjaControl);
    std::chrono::high_resolution_clock::time_point keyPressedTime;
    
    cocos2d::Label* labelTime;
    float remainingTime;

    void update(float dt);
    
    void goToGameOverScene(cocos2d::Ref *sender, GAME_RESULT result);
};

#endif // __GAME_SCENE_H__
