// forsettings.h - For Settings of the For Engine 2D

#pragma once

namespace FE2D {
	
	#pragma region Default Things
	
	// Default Texture - If Texture isn't Initialized
	#define FOR_DEFAULT_TEXTURE_PATH "Files/Application/Default/fe2d.png"
	// Default Normal Texture - If Normal Texture isn't Initialized
	#define FOR_DEFAULT_NORMAL_TEXTURE_PATH "Files/Textures/Default/Default.fornorm.png"
	
	// Default Shader - Basic Rendering without any Effect ( Except of Lighting and Normals )
	#define FOR_DEFAULT_SHADER_PATH "Files/Shaders/Default/Default"
	// Default Shader for Sprite Rendering - Basic Rendering without any Effect
	#define FOR_DEFAULT_SPRITE_SHADER_PATH "Files/Shaders/Sprite/Sprite"
	// Default Shader for Text Rendering - Basic Rendering without any Effect
	#define FOR_DEFAULT_TEXT_SHADER_PATH "Files/Shaders/Text/Text"
	
	// Game Name
	#define FOR_DEFAULT_GAME_NAME "Moss And Stone"
	
	// Game Icon
	#define FOR_DEFAULT_ICON_PATH "Files/Application/Icons/Icon.png"
	
	// A few Variants of Cursor
	#define FOR_DEFAULT_CURSOR_QUAD_PATH  "Files/Application/Cursors/CursorQuad.png"
	#define FOR_DEFAULT_CURSOR_POINT_PATH "Files/Application/Cursors/CursorPoint.png"
	
	// Default Text as wchar_t
	#define FOR_DEFAULT_TEXT L"Default Text"
	
	#pragma endregion
}