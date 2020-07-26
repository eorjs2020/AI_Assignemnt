#include "PathNode.h"
#include "Engine.h"

PathNode::PathNode(int x, int y, int w, int h) : m_bLOS(false)
{
	m_rDst.x = x;
	m_rDst.y = y;
	m_rDst.w = w;
	m_rDst.h = h;
}

PathNode::~PathNode()
{

}

void PathNode::Render()
{
	
}

void PathNode::Update()
{

}

glm::vec2 PathNode::GetPos()
{
	return glm::vec2(m_rDst.x, m_rDst.y);
}

float PathNode::GetWidth()
{
	return m_rDst.w;
}

float PathNode::GetHeight()
{
	return m_rDst.h;
}

void PathNode::setLOS(bool LOS)
{
	m_bLOS = LOS;
}

bool PathNode::getLOS() const
{
	return m_bLOS;
}
