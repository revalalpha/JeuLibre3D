#pragma once

enum struct EntityTag
{
	Body,
	BackWheels,
	Left_FrontWheel,
	Right_FrontWheel
};

struct TagComponent
{
	EntityTag tag;
};