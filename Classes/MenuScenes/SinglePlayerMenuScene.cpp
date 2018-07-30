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

#include "SinglePlayerMenuScene.h"
#include "GameScene.h"
#include "Definitions.h"

USING_NS_CC;

Scene* SinglePlayerMenuScene::createScene()
{
    return SinglePlayerMenuScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool SinglePlayerMenuScene::init()
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
    auto forestItem = MenuItemFont::create("Forest", CC_CALLBACK_1(SinglePlayerMenuScene::goToGameScene, this, FOREST));
    forestItem->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height * 0.6 + origin.y));
    forestItem->setColor(Color3B(0, 0, 0));
    forestItem->setFontSizeObj(42);
    auto desertItem = MenuItemFont::create("Desert", CC_CALLBACK_1(SinglePlayerMenuScene::goToGameScene, this, DESERT));
    desertItem->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height * 0.5 + origin.y));
    desertItem->setColor(Color3B(0, 0, 0));
    desertItem->setFontSizeObj(42);\
    auto cemeteryItem = MenuItemFont::create("Cemetery", CC_CALLBACK_1(SinglePlayerMenuScene::goToGameScene, this, CEMETERY));
    cemeteryItem->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height * 0.4 + origin.y));
    cemeteryItem->setColor(Color3B(0, 0, 0));
    cemeteryItem->setFontSizeObj(42);
    auto backItem = MenuItemFont::create("Go Back", CC_CALLBACK_1(SinglePlayerMenuScene::popScene, this));
    backItem->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height * 0.25 + origin.y));
    backItem->setColor(Color3B(0, 0, 0));
    backItem->setFontSizeObj(42);
    
    auto menu = Menu::create(forestItem, desertItem, cemeteryItem, backItem, NULL);
    menu->setPosition(Point::ZERO);
    this->addChild(menu);
   
    return true;
}


void SinglePlayerMenuScene::goToGameScene(cocos2d::Ref *sender, LEVEL level)
{
    auto gameScene = GameScene::createScene(level);
    Director::getInstance()->replaceScene(TransitionFade::create(TRASNSITION_TIME, gameScene));
}

void SinglePlayerMenuScene::popScene(cocos2d::Ref *sender)
{
    Director::getInstance()->popScene();
}
