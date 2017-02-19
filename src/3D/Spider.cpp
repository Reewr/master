#include "Spider.hpp"

Spider::Spider() : Logging::Log("Spider") {}

Spider::~Spider() {}

void Spider::update(float) {}

void Spider::drawShadow(Framebuffer*, Camera*) {}
void Spider::draw(Camera*) {}

void Spider::input(const Input::Event&) {}
