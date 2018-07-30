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

#include "Kunai.h"
#include "Definitions.h"

USING_NS_CC;


Kunai::Kunai(cocos2d::Layer *layer, cocos2d::Vec2 position, cocos2d::Vec2 velocity)
{
    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
    
    //create sprite
    kunaiSprite = Sprite::create("Kunai.png");
    kunaiSprite->setScale(KUNAI_SIZE / kunaiSprite->getContentSize().height);
    kunaiSprite->setPosition(position);
    kunaiSprite->setUserObject(this);
    
    kunaiBody = PhysicsBody::createBox(kunaiSprite->getContentSize(),PhysicsMaterial(10.0f, 0.1f, 0.0f));
    kunaiBody->setDynamic(true);
    kunaiBody->setMass(KUNAI_MASS);
    kunaiBody->setCollisionBitmask(KUNAI_COLLISION_BITMASK);
    kunaiBody->setContactTestBitmask(true);
    kunaiBody->setVelocity(velocity);
    int i = velocity.x > 0 ? -1 : 1;
    kunaiBody->setAngularVelocity(i * ANGULAR_VELOCITY);
    kunaiSprite->setPhysicsBody(kunaiBody);
    layer->addChild(kunaiSprite, 100);
}

