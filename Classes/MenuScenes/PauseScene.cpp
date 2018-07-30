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

#include "PauseScene.h"
#include "MainMenuScene.h"
#include "GameScene.h"
#include "Definitions.h"
#include <SimpleAudioEngine.h>

USING_NS_CC;

Scene* PauseScene::createScene()
{
    return PauseScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool PauseScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto backgroundSprite = Sprite::create("MenuBG.png", Rect(origin.x, origin.y, visibleSize.width, visibleSize.height));
    backgroundSprite->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    this->addChild(backgroundSprite);
    
    //menuitems
    auto retryItem = MenuItemFont::create("Resume", CC_CALLBACK_1(PauseScene::goToGameScene, this));
    retryItem->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height * 0.55 + origin.y));
    retryItem->setColor(Color3B(0, 0, 0));
    retryItem->setFontSizeObj(42);
    auto menuItem = MenuItemFont::create("Menu", CC_CALLBACK_1(PauseScene::goToMainMenuScene, this));
    menuItem->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height * 0.4 + origin.y));
    menuItem->setColor(Color3B(0, 0, 0));
    menuItem->setFontSizeObj(42);
    
    auto menu = Menu::create(retryItem, menuItem, NULL);
    menu->setPosition(Point::ZERO);
    this->addChild(menu);
    
    //sound
    auto audio = CocosDenshion::SimpleAudioEngine::getInstance();
    audio->resumeBackgroundMusic();
    
    return true;
}

void PauseScene::goToMainMenuScene(cocos2d::Ref *sender)
{
    auto scene = MainMenuScene::createScene();
    Director::getInstance()->replaceScene(TransitionFade::create(TRASNSITION_TIME, scene));
}

void PauseScene::goToGameScene(cocos2d::Ref *sender)
{
    auto audio = CocosDenshion::SimpleAudioEngine::getInstance();
    audio->pauseBackgroundMusic();
    Director::getInstance()->popScene();
}
