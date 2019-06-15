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

#include "GameScene.h"
#include "GameOverScene.h"
#include "PauseScene.h"
#include "Definitions.h"
#include "Ninja.h"
#include "Kunai.h"
#include <SimpleAudioEngine.h>
#include <ui/CocosGUI.h>

USING_NS_CC;

LEVEL currentLevel;
const std::string backgroundImageFiles[3] = {"ForestMap/BG.png", "DesertMap/BG.png", "CemeteryMap/BG.png"};
const std::string maps[3] = {"ForestMap/ForestMap.tmx", "DesertMap/DesertMap.tmx", "CemeteryMap/CemeteryMap.tmx"};

Scene* GameScene::createScene(LEVEL level)
{
    currentLevel = level;
    
    auto scene = Scene::createWithPhysics();
    scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_NONE);
    scene->getPhysicsWorld()->setGravity(Vec2(0.0f, -GRAVITY_FORCE));
    
    auto layer = GameScene::create();
    layer->setPhysicsWorld(scene->getPhysicsWorld());
    scene->addChild(layer);
    
    return scene;
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    
    //load background
    auto backgroundSprite = Sprite::create(backgroundImageFiles[currentLevel], Rect(origin.x, origin.y, visibleSize.width, visibleSize.height));
    backgroundSprite->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    this->addChild(backgroundSprite);
    
    //load map
    auto map = TMXTiledMap::create(maps[currentLevel]);
    auto parallaxNode = ParallaxNode::create();
    parallaxNode->addChild(map, 200, Vec2(0.5f, 0.5f), Vec2(0, 0));
    this->addChild(parallaxNode);
    collisionLayer = Layer::create();
    
    //load collision Objects
    auto collisionObjects = map->getObjectGroup("collision")->getObjects();
    for (auto obj : collisionObjects) {
        ValueMap mapObj = obj.asValueMap();
        float x = mapObj["x"].asFloat();
        float y = mapObj["y"].asFloat() ;
        float width = mapObj["width"].asFloat();
        float height = mapObj["height"].asFloat();
        
        auto groundBody = PhysicsBody::createBox(Size(width, height),PhysicsMaterial(0.1f, 1.0f, 0.0f));
        groundBody->setDynamic(false);
        auto ground = Sprite::create();
        ground->setPosition(Point(x + width/2,y + height/2));
        //attach groundBody to the sprite
        ground->setPhysicsBody(groundBody);
        collisionLayer->addChild(ground);
    }
    parallaxNode->addChild(collisionLayer, 2, Vec2(0.5f, 0.5f), Vec2(0, 0));
    float scaling = visibleSize.width / (map->getMapSize().width * map->getTileSize().width);
    parallaxNode->setScale(scaling);
    
    //load spawn
    auto respawnObjects = map->getObjectGroup("respawn")->getObjects();
    int index = rand() % respawnObjects.size();
    playerNinja = respawnNinja(respawnObjects.at(index), PLAYER);
    respawnObjects.erase(respawnObjects.begin() + index);
    for (auto obj : respawnObjects) {
        Ninja *ninja = respawnNinja(obj, BOT);
        enemies.push_back(ninja);
    }
    
    //Edge
    auto size = Size(visibleSize.width + NINJA_SIZE * 2, visibleSize.height + NINJA_SIZE * 2);
    auto edgeBody = PhysicsBody::createEdgeBox(size, PHYSICSBODY_MATERIAL_DEFAULT, 3 );
    auto edgeNode = Node::create();
    edgeNode->setPosition( Point( visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y ) );
    edgeNode->setPhysicsBody( edgeBody );
    edgeBody->setCollisionBitmask(BORDERS_COLLISION_BITMASK);
    edgeBody->setContactTestBitmask(true);
    this->addChild( edgeNode );
    
    
    //adds contact event listener
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(GameScene::onContactBegin, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
    
    //keyboard listener
    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
    keyboardListener->onKeyReleased = CC_CALLBACK_2(GameScene::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
    
    //timer
    remainingTime = GAME_TIME;
    labelTime = Label::createWithTTF("Time", "fonts/Marker Felt.ttf", 28);
    labelTime->setPosition(Point( visibleSize.width / 2 + origin.x, visibleSize.height + origin.y - labelTime->getContentSize().height));
    this->addChild(labelTime,1);
    
    this->scheduleUpdate();
    
    //sound
    auto audio = CocosDenshion::SimpleAudioEngine::getInstance();
    if (audio->isBackgroundMusicPlaying()) {
        audio->pauseBackgroundMusic();
    }
    
    //touchscreen
    #if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        loadTouchButtons();
    #endif
    
    return true;
}

Ninja* GameScene::respawnNinja(cocos2d::Value object, NINJA_CONTROL ninjaControl)
{
    ValueMap mapObj = object.asValueMap();
    float x = mapObj["x"].asFloat();
    float y = mapObj["y"].asFloat() ;
    
    return new Ninja(collisionLayer, Vec2(x, y), ninjaControl);
}

void GameScene::update(float dt) {
    playerNinja->update(dt);
    for(Ninja* enemy : enemies) {
        enemy->update(dt);
    }
    
    if (playerNinja->isDead()) {
        auto delayTimeAction = DelayTime::create(END_GAME_DELAY);
        auto callfuncAction = CallFunc::create([this]() {
            goToGameOverScene(this, DEFEAT);
        });
        this->runAction(Sequence::create(delayTimeAction, callfuncAction, nullptr));
    }
    
    bool enemiesAreDead = true;
    for (Ninja* enemy : enemies) {
        enemiesAreDead &= enemy->isDead();
    }
    if (enemiesAreDead) {
        auto delayTimeAction = DelayTime::create(END_GAME_DELAY);
        auto callfuncAction = CallFunc::create([this]() {
            goToGameOverScene(this, VICTORY);
        });
        this->runAction(Sequence::create(delayTimeAction, callfuncAction, nullptr));
    }
    
    if (remainingTime > 0.f) {
        remainingTime -= dt;
        labelTime->setString("Time Remaining: " + std::to_string((int)ceilf((remainingTime))) + " s");
        if (remainingTime <= 0.f) {
            auto delayTimeAction = DelayTime::create(END_GAME_DELAY);
            auto callfuncAction = CallFunc::create([this]() {
                goToGameOverScene(this, DEFEAT);
            });
            this->runAction(Sequence::create(delayTimeAction, callfuncAction, nullptr));
        }
    }
}

bool GameScene::onContactBegin(PhysicsContact& contact)
{
    PhysicsBody *bodyA = contact.getShapeA()->getBody();
    PhysicsBody *bodyB = contact.getShapeB()->getBody();
    
    if (bodyA->getCollisionBitmask() == NINJA_COLLISION_BITMASK && bodyB->getCollisionBitmask() == KUNAI_COLLISION_BITMASK) {
        auto ninja = (Ninja*)bodyA->getOwner()->getUserObject();
        ninja->applyDamageFrom(bodyB);
    } else if (bodyA->getCollisionBitmask() == KUNAI_COLLISION_BITMASK && bodyB->getCollisionBitmask() == NINJA_COLLISION_BITMASK) {
        auto ninja = (Ninja*)bodyB->getOwner()->getUserObject();
        ninja->applyDamageFrom(bodyA);
    }
    if (bodyA->getCollisionBitmask() == NINJA_COLLISION_BITMASK && bodyB->getCollisionBitmask() == BORDERS_COLLISION_BITMASK) {
        auto ninja = (Ninja*)bodyA->getOwner()->getUserObject();
        ninja->die();
    } else if (bodyA->getCollisionBitmask() == BORDERS_COLLISION_BITMASK && bodyB->getCollisionBitmask() == NINJA_COLLISION_BITMASK) {
        auto ninja = (Ninja*)bodyB->getOwner()->getUserObject();
        ninja->die();
    }
    return true;
}

void GameScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event * event)
{
    if (keyCode == EventKeyboard::KeyCode::KEY_A)
    {
        playerNinja->run(-NINJA_VELOCITY);
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_D)
    {
        playerNinja->run(NINJA_VELOCITY);
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_W){
        playerNinja->jump();
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_SPACE){
        keyPressedTime = std::chrono::high_resolution_clock::now();
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW){
        playerNinja->setAimSpeed(-AIM_ANGLE_SPEED);
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW){
        playerNinja->setAimSpeed(AIM_ANGLE_SPEED);
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE){
        auto scene = PauseScene::createScene();
        Director::getInstance()->pushScene(TransitionFade::create(TRASNSITION_TIME, scene));
    }
}

void GameScene::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event * event)
{
    if (keyCode == EventKeyboard::KeyCode::KEY_A || keyCode == EventKeyboard::KeyCode::KEY_D)
    {
        playerNinja->stand();
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW || keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW){
        playerNinja->setAimSpeed(0);
    } else if (keyCode == EventKeyboard::KeyCode::KEY_SPACE){
        float duration = std::chrono::duration_cast<std::chrono::milliseconds>
        (std::chrono::high_resolution_clock::now() - keyPressedTime).count();
        playerNinja->throwKunai(MIN(duration, MAX_THROW_POWER));
    }
}

void GameScene::goToGameOverScene(cocos2d::Ref *sender, GAME_RESULT result)
{
    auto scene = GameOverScene::createScene(currentLevel, result);
    Director::getInstance()->replaceScene(TransitionFade::create(TRASNSITION_TIME, scene));
}

void GameScene::loadTouchButtons()
{
    ui::Button *leftButton = ui::Button::create("Buttons/leftButton.png", "Buttons/leftPressedButton.png");
    leftButton->setScale(BUTTONSIZE/leftButton->getContentSize().width);
    leftButton->setPosition(Vec2(BUTTONSIZE / 2 , BUTTONSIZE / 2));
    this->addChild(leftButton);
    leftButton->addTouchEventListener(
                                      [this](Ref *sender, ui::Widget::TouchEventType type) {
                                          switch (type) {
                                              case ui::Widget::TouchEventType::BEGAN:
                                                  playerNinja->run(-NINJA_VELOCITY);
                                                  break;
                                              case ui::Widget::TouchEventType::ENDED:
                                                  playerNinja->stand();
                                                  break;
                                              case ui::Widget::TouchEventType::CANCELED:
                                                  playerNinja->stand();
                                                  break;
                                              default:
                                                  break;
                                          }
                                      }
                                      );
    
    ui::Button *rightButton = ui::Button::create("Buttons/rightButton.png", "Buttons/rightPressedButton.png");
    rightButton->setScale(BUTTONSIZE/rightButton->getContentSize().width);
    rightButton->setPosition(Vec2(BUTTONSIZE * 2, BUTTONSIZE / 2 ));
    this->addChild(rightButton);
    rightButton->addTouchEventListener(
                                       [this](Ref *sender, ui::Widget::TouchEventType type) {
                                           switch (type) {
                                               case ui::Widget::TouchEventType::BEGAN:
                                                   playerNinja->run(NINJA_VELOCITY);
                                                   break;
                                               case ui::Widget::TouchEventType::ENDED:
                                                   playerNinja->stand();
                                                   break;
                                               case ui::Widget::TouchEventType::CANCELED:
                                                   playerNinja->stand();
                                                   break;
                                               default:
                                                   break;
                                           }
                                       }
                                       );
    
    ui::Button *pauseButton = ui::Button::create("Buttons/pauseButton.png", "Buttons/pausePressedButton.png");
    pauseButton->setScale(BUTTONSIZE/pauseButton->getContentSize().width);
    pauseButton->setPosition(Vec2(BUTTONSIZE/1.5, visibleSize.height - BUTTONSIZE/1.5));
    this->addChild(pauseButton);
    pauseButton->addTouchEventListener(
                                       [](Ref *sender, ui::Widget::TouchEventType type) {
                                           switch (type) {
                                               case ui::Widget::TouchEventType::BEGAN: {
                                                   Scene* scene = PauseScene::createScene();
                                                   Director::getInstance()->pushScene(TransitionFade::create(TRASNSITION_TIME, scene));
                                                   break;
                                               }
                                               default:
                                                   break;
                                           }
                                       }
                                       );
    
    ui::Button *jumpButton = ui::Button::create("Buttons/jumpButton.png", "Buttons/jumpPressedButton.png");
    jumpButton->setScale(BUTTONSIZE/jumpButton->getContentSize().width);
    jumpButton->setPosition(Vec2(BUTTONSIZE * 1.25, BUTTONSIZE * 1.5));
    this->addChild(jumpButton);
    jumpButton->addTouchEventListener(
                                      [this](Ref *sender, ui::Widget::TouchEventType type) {
                                          switch (type) {
                                              case ui::Widget::TouchEventType::BEGAN:
                                                  playerNinja->jump();
                                                  break;
                                              default:
                                                  break;
                                          }
                                      }
                                      );
    
    ui::Button *throwButton = ui::Button::create("Buttons/throwButton.png", "Buttons/throwPressedButton.png");
    throwButton->setScale(BUTTONSIZE/throwButton->getContentSize().width);
    throwButton->setPosition(Vec2(visibleSize.width - BUTTONSIZE/2, BUTTONSIZE));
    this->addChild(throwButton);
    throwButton->addTouchEventListener(
                                       [this](Ref *sender, ui::Widget::TouchEventType type) {
                                           switch (type) {
                                               case ui::Widget::TouchEventType::BEGAN:
                                                   keyPressedTime = std::chrono::high_resolution_clock::now();
                                                   break;
                                               case ui::Widget::TouchEventType::ENDED: {
                                                   float duration = std::chrono::duration_cast<std::chrono::milliseconds>
                                                   (std::chrono::high_resolution_clock::now() - keyPressedTime).count();
                                                   playerNinja->throwKunai(MIN(duration, MAX_THROW_POWER));
                                                   break;
                                               }
                                               case ui::Widget::TouchEventType::CANCELED:
                                                   playerNinja->stand();
                                                   break;
                                               default:
                                                   break;
                                           }
                                       }
                                       );
    
    ui::Button *aimButtonUpButton = ui::Button::create("Buttons/jumpButton.png", "Buttons/jumpPressedButton.png");
    aimButtonUpButton->setScale(BUTTONSIZE/aimButtonUpButton->getContentSize().width);
    aimButtonUpButton->setPosition(Vec2(visibleSize.width - BUTTONSIZE * 1.5, BUTTONSIZE * 1.5));
    this->addChild(aimButtonUpButton);
    aimButtonUpButton->addTouchEventListener(
                                      [this](Ref *sender, ui::Widget::TouchEventType type) {
                                          switch (type) {
                                              case ui::Widget::TouchEventType::BEGAN:
                                                  playerNinja->setAimSpeed(AIM_ANGLE_SPEED);
                                                  break;
                                              case ui::Widget::TouchEventType::ENDED:
                                                  playerNinja->setAimSpeed(0);
                                                  break;
                                              case ui::Widget::TouchEventType::CANCELED:
                                                playerNinja->setAimSpeed(0);
                                                  break;
                                              default:
                                                  break;
                                          }
                                      }
                                      );
    
    ui::Button *aimButtonDownButton = ui::Button::create("Buttons/downButton.png", "Buttons/downPressedButton.png");
    aimButtonDownButton->setScale(BUTTONSIZE/aimButtonDownButton->getContentSize().width);
    aimButtonDownButton->setPosition(Vec2(visibleSize.width - BUTTONSIZE * 1.5, BUTTONSIZE /2));
    this->addChild(aimButtonDownButton);
    aimButtonDownButton->addTouchEventListener(
                                             [this](Ref *sender, ui::Widget::TouchEventType type) {
                                                 switch (type) {
                                                     case ui::Widget::TouchEventType::BEGAN:
                                                         playerNinja->setAimSpeed(-AIM_ANGLE_SPEED);
                                                         break;
                                                     case ui::Widget::TouchEventType::ENDED:
                                                         playerNinja->setAimSpeed(0);
                                                         break;
                                                     case ui::Widget::TouchEventType::CANCELED:
                                                         playerNinja->setAimSpeed(0);
                                                         break;
                                                     default:
                                                         break;
                                                 }
                                             }
                                             );
}
