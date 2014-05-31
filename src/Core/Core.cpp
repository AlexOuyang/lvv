//
//  Core.cpp
//  CSE168_Rendering
//
//  Created by Gael Jochaud du Plessix on 4/5/14.
//
//

#include "Core.h"

const float Core::Epsilon = 0.0001f;

int Core::roundUpPow2(int n) {
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return n+1;
}

std::string Core::baseDirectory = "";

void Core::setBaseDirectory(const std::string& dir) {
    baseDirectory = dir;
}

QDebug operator<<(QDebug dbg, const std::string& s) {
    return dbg << s.c_str();
}

QDebug operator<<(QDebug dbg, const vec2& v) {
    dbg.nospace() << "(" << v.x << ", " << v.y << ")";
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const vec3& v) {
    dbg.nospace() << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const vec4& v) {
    dbg.nospace() << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const mat4x4& m) {
    dbg << "(";
    dbg << m[0];
    dbg << m[1];
    dbg << m[2];
    dbg << m[3];
    dbg << ")";
    return dbg;
}

QDebug operator<<(QDebug dbg, const quat& q) {
    dbg.nospace() << "(" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << ")";
    return dbg.space();
}