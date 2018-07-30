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

#include "Ninja.h"
#include "Definitions.h"
#include "AnimationManager.h"
#include "Kunai.h"

USING_NS_CC;


Ninja::Ninja(cocos2d::Layer *layer, cocos2d::Vec2 position, NINJA_CONTROL ninjaControl)
{
    this->layer = layer;
    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
    
    //load animations
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Ninja/run/run.plist");
    runFrames = AnimationManager::getAnimation("Run__%03d.png", NINJA_FRAMES_COUNT-1);
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Ninja/jump/jump.plist");
    jumpFrames = AnimationManager::getAnimation("Jump__%03d.png", NINJA_FRAMES_COUNT-1);
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Ninja/idle/idle.plist");
    idleFrames = AnimationManager::getAnimation("Idle__%03d.png", NINJA_FRAMES_COUNT-1);
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Ninja/throw/throw.plist");
    throwFrames = AnimationManager::getAnimation("Throw__%03d.png", NINJA_FRAMES_COUNT-1);
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Ninja/dead/dead.plist");
    deadFrames = AnimationManager::getAnimation("Dead__%03d.png", NINJA_FRAMES_COUNT-1);
    
    //create sprite
    ninjaSprite = Sprite::create("Ninja.png");
    ninjaSprite->setScale(NINJA_SIZE / ninjaSprite->getContentSize().height);
    ninjaSprite->setPosition(position);
    ninjaSprite->setUserObject(this);
    
    //create PhysicsBody
    contentSize = ninjaSprite->getContentSize();
    ninjaBody = PhysicsBody::createBox(contentSize,PhysicsMaterial(1000.0f, 0.0f, 0.0f));
    ninjaBody->setDynamic(true);
    ninjaBody->setMass(NINJA_MASS);
    ninjaBody->setRotationEnable(false);
    ninjaBody->setCollisionBitmask(NINJA_COLLISION_BITMASK);
    ninjaBody->setContactTestBitmask(true);
    ninjaSprite->setPhysicsBody(ninjaBody);
    layer->addChild(ninjaSprite, 100);
    
    setAnimation(idleFrames, FOREVER, false);
    
    this->ninjaControl = ninjaControl;
    //load aim
    throwProperties.angle = 0;
    throwProperties.angleSpeed = 0;
    aimSprite = Sprite::create("aim.png");
    layer->addChild(aimSprite, 99);
    
    //set hp
    hp = NINJA_MAX_HP;
    dead = false;
    hpLine = DrawNode::create();
    ninjaSprite->addChild(hpLine);
}

void Ninja::run(float velocity)
{
    if(dead) {return;}
    auto curVelocity = ninjaBody->getVelocity();
    auto newVelocity = Vec2(velocity, curVelocity.y);
    ninjaBody->setVelocity(newVelocity);
  
    ninjaSprite->stopAllActions();
    if(velocity > 0) {
        setAnimation(runFrames, FOREVER, false);
    } else if (velocity < 0) {
        setAnimation(runFrames, FOREVER, true);
    }
}

void Ninja::stand()
{
    if(dead) {return;}
    auto curVelocity = ninjaBody->getVelocity();
    auto newVelocity = Vec2(0, curVelocity.y);
    ninjaBody->setVelocity(newVelocity);
    
    ninjaSprite->stopAllActions();
    if(curVelocity.x > 0) {
        setAnimation(idleFrames, FOREVER, false);
    } else if (curVelocity.x < 0) {
        setAnimation(idleFrames, FOREVER, true);
    }
}

void Ninja::jump()
{
    if(dead) {return;}
    if(ninjaBody->getVelocity().y < 0.1 && ninjaBody->getVelocity().y > -0.1) {
        ninjaSprite->stopAllActions();
        Vec2 force = Vec2(0, NINJA_JUMP_FORCE);
        ninjaBody->applyImpulse(force);
        
        int i = ninjaSprite->isFlipX()? -1 : 1;
        if(ninjaSprite->isFlipX()) {
            setAnimation(jumpFrames, ONE, true);
        } else {
            setAnimation(jumpFrames, ONE, false);
        }
    }
}

void Ninja::throwKunai(float force)
{
    if(dead) {return;}
    auto ninjaPos = ninjaSprite->getPosition();
    ninjaSprite->stopAllActions();
    int i = ninjaSprite->isFlipX()? -1 : 1;
    if(ninjaSprite->isFlipX()) {
        setAnimation(throwFrames, ONE, true);
    } else {
        setAnimation(throwFrames, ONE, false);
    }
    auto kunaiVelocity = Vec2(i * cos(throwProperties.angle* M_PI / 180) * force, sin(throwProperties.angle* M_PI / 180) * force);
    Kunai *kunai = new Kunai(layer, throwProperties.aimPosition, kunaiVelocity);
}

void Ninja::setAnimation(cocos2d::Vector<cocos2d::SpriteFrame*> frames, RepeatTimes repeatTimes, bool flipped)
{
    ninjaSprite->setSpriteFrame(frames.front());
    auto animation = Animation::createWithSpriteFrames(frames, 1.0f/NINJA_FRAMES_COUNT);
    
    switch (repeatTimes) {
        case ONE:
            ninjaSprite->runAction(Repeat::create(Animate::create(animation), 1));
            break;
        case FOREVER:
            ninjaSprite->runAction(RepeatForever::create(Animate::create(animation)));
            break;
        default:
            break;
    }
    ninjaSprite->setFlippedX(flipped);
}

void Ninja::applyDamageFrom(cocos2d::PhysicsBody *body) {
    float damage = sqrt(body->getVelocity().x * body->getVelocity().x + body->getVelocity().y * body->getVelocity().y);
    
    hp -= damage;
    if (hp < 0 && !dead) {
        die();
    }
}

void Ninja::die()
{
    ninjaSprite->stopAllActions();
    hp = 0;
    dead = true;
    int i = ninjaSprite->isFlipX()? -1 : 1;
    if(ninjaSprite->isFlipX()) {
        setAnimation(deadFrames, ONE, true);
    } else {
        setAnimation(deadFrames, ONE, false);
    }
}

void Ninja::drawHpLine()
{
    hpLine->clear();
    float damage = (NINJA_MAX_HP - hp)/NINJA_MAX_HP * contentSize.width;
    Vec2 vertices[] =
    {
        Vec2(0,contentSize.height+15),
        Vec2(MAX(contentSize.width - damage, 0),contentSize.height+15),
        Vec2(MAX(contentSize.width - damage, 0),contentSize.height),
        Vec2(0,contentSize.height)
    };
    hpLine->drawPolygon(vertices, 4, Color4F(0.0f,1.0f,0.0f,1), 3, Color4F(0.2f,0.2f,0.2f,1));
}

void Ninja::update(float dt) {
    drawHpLine();
    //Aim movement
    if (this->ninjaControl == BOT) { return; }
    auto ninjaPosition = ninjaSprite->getPosition();
    int i = ninjaSprite->isFlipX()? -1 : 1;
    throwProperties.aimPosition = Vec2(ninjaPosition.x + NINJA_SIZE/2, ninjaPosition.y + NINJA_SIZE/2);
    throwProperties.angle = MIN(throwProperties.angle, MAX_AIM_ANGLE);
    throwProperties.angle = MAX(throwProperties.angle, MIN_AIM_ANGLE);
    throwProperties.angle = throwProperties.angle + throwProperties.angleSpeed *dt;
    throwProperties.aimPosition = Vec2(throwProperties.aimPosition.x + i * AIM_RADIUS * cos(throwProperties.angle* M_PI / 180),throwProperties.aimPosition.y + AIM_RADIUS * sin(throwProperties.angle* M_PI / 180));
    aimSprite->setPosition(throwProperties.aimPosition);
}
