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

#ifndef __NINJA_H__
#define __NINJA_H__

#include "cocos2d.h"
#include "Kunai.h"
#include "Definitions.h"

enum RepeatTimes {FOREVER, ONE};
struct THROW_PROPERTIES {
    cocos2d::Vec2 aimPosition;
    float angle;
    float angleSpeed;
};

class Ninja : cocos2d::Ref
{
public:
    Ninja(cocos2d::Layer *layer, cocos2d::Vec2 position, NINJA_CONTROL ninjaControl);
    void run(float velocity);
    void stand();
    void jump();
    void throwKunai(float force);
    void applyDamageFrom(cocos2d::PhysicsBody *body);
    bool isDead() { return dead; }
    void die();
    
    void setAimSpeed(float speed) {throwProperties.angleSpeed = speed;};
    void update(float dt);
    
private:
    cocos2d::Layer *layer;
    cocos2d::Size visibleSize;
    cocos2d::Vec2 origin;
    
    cocos2d::Size contentSize;
    cocos2d::Sprite *ninjaSprite;
    cocos2d::PhysicsBody *ninjaBody;
    
    cocos2d::Sprite *aimSprite;
    THROW_PROPERTIES throwProperties;
    
    cocos2d::Vector<cocos2d::SpriteFrame*> runFrames;
    cocos2d::Vector<cocos2d::SpriteFrame*> jumpFrames;
    cocos2d::Vector<cocos2d::SpriteFrame*> idleFrames;
    cocos2d::Vector<cocos2d::SpriteFrame*> throwFrames;
    cocos2d::Vector<cocos2d::SpriteFrame*> deadFrames;
    void setAnimation(cocos2d::Vector<cocos2d::SpriteFrame*> frames, RepeatTimes repeatTimes, bool flipped);
    
    NINJA_CONTROL ninjaControl;
    float hp;
    bool dead;
    void drawHpLine();
    cocos2d::DrawNode* hpLine;
};

#endif // __NINJA_H__
