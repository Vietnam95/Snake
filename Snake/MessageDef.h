#pragma once
#include "stdafx.h"
#include "Common.h"

// Class request update direction message
class MsgDirectionUpdateReq
{
protected:
    ControlLogicType    m_enmType;          // Type of control logic
    Direction           m_enmDirection;     // Direction
public:
    // Contructor
    MsgDirectionUpdateReq()
    {
        clear();
    }

    // Contructor with param
    MsgDirectionUpdateReq(const ControlLogicType enmType, const Direction enmDirection)
        :m_enmType(enmType)
        , m_enmDirection(enmDirection)
    {
    }
    
    // Get type of Control Logic
    const ControlLogicType getControlLogicType() const
    {
        return m_enmType;
    }

    // Get direction
    const Direction getDirection() const
    {
        return m_enmDirection;
    }

    friend class boost::serialization::access;
    // Convert msg to binary
    const std::vector<char> toBinary() const
    {
        std::ostringstream stream;
        boost::archive::binary_oarchive archive(stream, boost::archive::no_header);
        archive << *this;

        std::string adapter = stream.str();
        return std::vector<char>(adapter.begin(), adapter.end());
    }

protected:
    // Boost serialize
    template <typename Archive>
    void serialize(Archive& ar, const unsigned int)
    {
        ar& m_enmType;
        ar& m_enmDirection;
    }

    // Clear class to default
    void clear()
    {
        m_enmType = ControlLogicType::Single;
        m_enmDirection = Direction::up;
    }

    // operator = override
    MsgDirectionUpdateReq operator = (const MsgDirectionUpdateReq& another)
    {
        this->m_enmDirection = another.m_enmDirection;
        this->m_enmType = another.m_enmType;
    }
};
BOOST_CLASS_VERSION(MsgDirectionUpdateReq, 1)