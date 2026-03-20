#pragma once
#include <array>
#include <span>
#include <utility>

class CarPhysic
{
public:


private:
	std::pair<int, int> Shift_1 = std::make_pair(0, 2);
	std::pair<int, int> Shift_2 = std::make_pair(2, 4);
	std::pair<int, int> Shift_3 = std::make_pair(4, 8);
	std::pair<int, int> Shift_4 = std::make_pair(8, 11);
	std::pair<int, int> Shift_5 = std::make_pair(11, 20);

	std::array<std::pair<int, int>, 5> Shifts = {
		Shift_1, Shift_2, Shift_3, Shift_4, Shift_5
	};
};