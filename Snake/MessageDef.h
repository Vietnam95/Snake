#pragma once
#include "stdafx.h"
#include "Common.h"

enum MessageId
{
    MsgNone = 0
    , DirectionUpdateReq
    , SynchroReq
    , WinningNotice
};

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

    // show details as string
    const std::string toString() const
    {
        return boost::io::str(boost::format("MsgDirectionUpdateReq, ControlLogicType: %1%, Direction: %2%") % EToString(m_enmType) % static_cast<uint32_t>(m_enmDirection));
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
        m_enmType = ControlLogicType::None;
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


// Class request synchro message
class MsgSnake
{
protected:
    ControlLogicType                           m_enmType;          // Type of control logic
    Direction                                  m_enmDirection;     // Direction
    std::vector<Point>                         m_lstSnake;         // Snake point
    Point                                      m_objPreTail;       // Pre tail of snake

public:
    // Contructor
    MsgSnake() 
    {
        clear();
    }

    // Contructor with param
    MsgSnake(const ControlLogicType                enmType
        , const Direction                               enmDirection
        , const std::vector<Point>& lstSnake
        , const Point& objPreTail)
        :m_enmType(enmType)
        , m_enmDirection(enmDirection)
        , m_lstSnake(lstSnake)
        , m_objPreTail(objPreTail)
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

    // Get snake
    const std::vector<Point>& getSnake() const
    {
        return m_lstSnake;
    }

    // Get pre tail of snake
    const Point& getPreTail() const
    {
        return m_objPreTail;
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

    // show details as string
    const std::string toString() const
    {
        std::string strSnake = "";

        for (const Point& snake : m_lstSnake)
        {
            if (&snake != &m_lstSnake.front())
            {
                strSnake += ", ";
            }
            strSnake += snake.toString();
        }

        return boost::io::str(boost::format("MsgSnake, ControlLogicType: %1%, Direction: %2%, PreTail: %3%, Snake: [%4%]")
            % EToString(m_enmType)
            % static_cast<uint32_t>(m_enmDirection)
            % m_objPreTail.toString()
            % strSnake);
    }

protected:
    // Boost serialize
    template <typename Archive>
    void serialize(Archive& ar, const unsigned int)
    {
        ar& m_enmType;
        ar& m_enmDirection;
        ar& m_lstSnake;
        ar& m_objPreTail;
    }

    // Clear class to default
    void clear()
    {
        m_enmType = ControlLogicType::None;
        m_enmDirection = Direction::up;
        m_lstSnake = std::vector<Point>();
        m_objPreTail = Point();
    }

    // operator = override
    //MsgSnake operator = (const MsgSnake& another)
    //{
    //    this->m_enmType = another.m_enmType;
    //    this->m_enmDirection = another.m_enmDirection;
    //    this->m_lstSnake = another.m_lstSnake;
    //    this->m_objPreTail = another.m_objPreTail;

    //}
};
BOOST_CLASS_VERSION(MsgSnake, 1)

// Class request synchro message
class MsgSynchroReq
{
protected:
    std::vector<MsgSnake>                      m_lstSnake;         // snake list
    std::vector<Point>                         m_lstApple;         // Apple list
    std::map<ControlLogicType, uint32_t>       m_lstScore;         // Score list

public:
    // Contructor
    MsgSynchroReq()
    {
        clear();
    }

    // Contructor with param
    MsgSynchroReq(const std::vector<MsgSnake>&          lstSnake
        , const std::vector<Point>&                     lstApple
        , const std::map<ControlLogicType, uint32_t>&   lstScore)
        : m_lstSnake(lstSnake)
        , m_lstApple(lstApple)
        , m_lstScore(lstScore)
    {
    }

    // Get snake
    const std::vector<MsgSnake>& getSnake() const
    {
        return m_lstSnake;
    }

    // Get apple
    const std::vector<Point>& getApple() const
    {
        return m_lstApple;
    }

    // Get apple
    const std::map<ControlLogicType, uint32_t> getScore() const
    {
        return m_lstScore;
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

    // show details as string
    const std::string toString() const
    {
        std::string strSnake = "";

        for (const auto& snake : m_lstSnake)
        {
            if (&snake != &m_lstSnake.front())
            {
                strSnake += ", ";
            }
            strSnake += snake.toString();
        }

        std::string strApple = "";

        for (const Point& apple : m_lstApple)
        {
            if (&apple != &m_lstApple.front())
            {
                strApple += ", ";
            }
            strApple += apple.toString();
        }

        std::string strScore = "";

        for (const auto& score : m_lstScore)
        {
            if (score != *m_lstScore.begin())
            {
                strScore += ", ";
            }
            strScore += EToString(score.first) + " : " + std::to_string(score.second);
        }

        return boost::io::str(boost::format("MsgSynchroReq, Snake[%1%], Apple: [%2%] , Score: [%3%]")
            % strSnake
            % strApple
            % strScore);
    }

protected:
    // Boost serialize
    template <typename Archive>
    void serialize(Archive& ar, const unsigned int)
    {
        ar& m_lstSnake;
        ar& m_lstApple;
        ar& m_lstScore;
    }

    // Clear class to default
    void clear()
    {
        m_lstSnake      = std::vector<MsgSnake>();
        m_lstApple      = std::vector<Point>();
        m_lstScore      = std::map<ControlLogicType, uint32_t>();

    }

    // operator = override
    MsgSynchroReq operator = (const MsgSynchroReq& another)
    {
        this->m_lstSnake = another.m_lstSnake;
        this->m_lstApple = another.m_lstApple;
        this->m_lstScore = another.m_lstScore;

    }
};
BOOST_CLASS_VERSION(MsgSynchroReq, 1)

// Class winning notice message
class MsgWinningNotice
{
protected:
    ControlLogicType    m_enmType;          // Type of control logic
public:
    // Contructor
    MsgWinningNotice()
    {
        clear();
    }

    // Contructor with param
    MsgWinningNotice(const ControlLogicType enmType)
        :m_enmType(enmType)
    {
    }

    // Get type of Control Logic
    const ControlLogicType getControlLogicType() const
    {
        return m_enmType;
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

    // show details as string
    const std::string toString() const
    {
        return boost::io::str(boost::format("MsgWinningNotice, From: %1%") % EToString(m_enmType));
    }

protected:
    // Boost serialize
    template <typename Archive>
    void serialize(Archive& ar, const unsigned int)
    {
        ar& m_enmType;
    }

    // Clear class to default
    void clear()
    {
        m_enmType = ControlLogicType::None;
    }

    // operator = override
    MsgWinningNotice operator = (const MsgWinningNotice& another)
    {
        this->m_enmType = another.m_enmType;
    }
};
BOOST_CLASS_VERSION(MsgWinningNotice, 1)

// Class wrap message
class MsgWrapBase
{
protected:
    MessageId    m_enmMessageId;          // Message id
    std::vector<char> m_binaryMsg;        // binary msg
public:
    // Contructor
    MsgWrapBase()
    {
        clear();
    }

    // Contructor with param
    MsgWrapBase(const MessageId enmMsgId, const std::vector<char>& charMsg)
        :m_enmMessageId(enmMsgId)
        , m_binaryMsg(charMsg)
    {
    }

    // Get msg id
    const MessageId getMessageId() const
    {
        return m_enmMessageId;
    }

    // Get msg
    const std::vector<char> getMessage() const
    {
        return m_binaryMsg;
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

    // show details as string
    const std::string toString() const
    {
        return boost::io::str(boost::format("MsgWrapBase, MessageId: %1%, Msg size:[%2%]") % static_cast<uint32_t>(m_enmMessageId) % m_binaryMsg.size());
    }

protected:
    // Boost serialize
    template <typename Archive>
    void serialize(Archive& ar, const unsigned int)
    {
        ar& m_enmMessageId;
        ar& m_binaryMsg;
    }

    // Clear class to default
    void clear()
    {
        m_enmMessageId = MessageId::MsgNone;
        m_binaryMsg = std::vector<char>();
    }

    // operator = override
    MsgWrapBase operator = (const MsgWrapBase& another)
    {
        this->m_enmMessageId = another.m_enmMessageId;
        this->m_binaryMsg = another.m_binaryMsg;
    }
};
BOOST_CLASS_VERSION(MsgWrapBase, 1)