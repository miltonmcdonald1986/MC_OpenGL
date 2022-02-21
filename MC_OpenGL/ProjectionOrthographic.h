#pragma once


#include <glm.hpp>
#include <gtc/matrix_transform.hpp>


namespace MC_OpenGL
{


	class ProjectionOrthographic
	{
	public:
		ProjectionOrthographic();

		auto ProjectionMatrix () const -> glm::mat4;

		auto GetBottom() const -> float
		{
			return m_Bottom;
		}

		auto GetFar() const -> float
		{
			return m_Far;
		}

		auto GetLeft() const -> float
		{
			return m_Left;
		}

		auto GetNear() const -> float
		{
			return m_Near;
		}

		auto GetRight() const -> float
		{
			return m_Right;
		}

		auto GetTop() const -> float
		{
			return m_Top;
		}

		auto SetBottom(float value) -> void
		{
			m_Bottom = value;
		}

		auto SetFar(float value) -> void
		{
			m_Far = value;
		}

		auto SetLeft(float value) -> void
		{
			m_Left = value;
		}

		auto SetNear(float value) -> void
		{
			m_Near = value;
		}

		auto SetRight(float value) -> void
		{
			m_Right = value;
		}

		auto SetTop(float value) -> void
		{
			m_Top = value;
		}

	private:
		float m_Left	= -400.f;
		float m_Right	=  400.f;
		float m_Bottom	= -300.f;
		float m_Top		=  300.f;
		float m_Near	=    0.1f;
		float m_Far		=  100.f;
	};


}
