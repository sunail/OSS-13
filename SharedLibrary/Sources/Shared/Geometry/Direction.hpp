#pragma once

namespace uf {
    template<typename T>
    struct vec3;

    typedef vec3<int> rpos;

	enum class Direction : char {
		NONE  = 0,
		SOUTH = 1 << 0,
		WEST  = 1 << 1,
		NORTH = 1 << 2,
		EAST  = 1 << 3,
		UP    = 1 << 4,
		DOWN  = 1 << 5,
		SOUTH_WEST = SOUTH|WEST,
		NORTH_WEST = NORTH|WEST,
		NORTH_EAST = NORTH|EAST,
		SOUTH_EAST = SOUTH|EAST
	};
	Direction operator |(Direction l, Direction r);
	Direction operator &(Direction l, Direction r);

	int log2(Direction dir);

	Direction VectToDirection(rpos);
	rpos DirectionToVect(Direction);
}