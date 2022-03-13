#include "SocketWrapperPCH.hpp"
#include "Player.hpp"

uint32_t Player::GetHealth() const
{
	return m_health;
}

uint32_t Player::GetAmmo() const
{
	return m_ammo;
}

void Player::Write(OutputMemoryBitStream& out_stream) const
{
	out_stream.Write(m_health, 4); //1010 = 10					(4 bits)
	out_stream.Write(m_ammo, 2); //11 = 3						(2 bits)
	out_stream.Write(m_name); //											(64 bits {"Default" + '\0'})
	out_stream.Write(m_position); //limit? game dependent					(96 bits)
	out_stream.Write(m_rotation); //										(49 bits)
	out_stream.Write(m_weapons, 7); //biggest == 100			(5 * 7)	(99 bits {32 size, 32 bitsPerElement, 35 bitsFromElements})

	//																		(314 bits TOTAL -> 40 bytes)
	//																		(320 bits nearest byte)
}

void Player::Read(InputMemoryBitStream& in_stream)
{
	in_stream.Read(m_health, 4);
	in_stream.Read(m_ammo, 2);
	in_stream.Read(m_name);
	in_stream.Read(m_position);
	in_stream.Read(m_rotation);
	in_stream.Read(m_weapons, 7);
}

void Player::toString() const
{
	std::cout << m_name << " has Health: " << m_health << ", Ammo: " << m_ammo <<
		" Position: " << "(" << m_position.mX << "," <<
		m_position.mY << "," << m_position.mZ << ")" << " Quaternion: " <<
		"(" << m_rotation.mX << "," << m_rotation.mY << "," << m_rotation.mZ << "," << m_rotation.mW << ")" << std::endl;
	for (int i = 0; i < m_weapons.size(); i++)
	{
		std::cout << m_weapons[i] << std::endl;
	}
}
