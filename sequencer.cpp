#include "sequencer.h"
#include <QtDebug>

Sequencer::Sequencer(QObject *parent) : QObject(parent)
{
    m_seq_state = SEQ_T1;
}

Sequencer::~Sequencer()
{
}

void Sequencer::update()
{
    switch(m_seq_state)
    {
        case SEQ_T1:
            m_seq_state = SEQ_T2;
        break;

        case SEQ_T2:
            m_seq_state = SEQ_T3;
        break;

        case SEQ_T3:
            m_seq_state = SEQ_T1;
        break;

        default: m_seq_state = SEQ_T1;
            break;
    }
    qDebug() << m_seq_state;
}

t_seq_state Sequencer::getStep()
{
    return m_seq_state;
}

t_seq_state Sequencer::STEP_T1()
{
    QString command = "R";
    emit getData(command.toLocal8Bit());
    return SEQ_T2;
}

t_seq_state Sequencer::STEP_T2()
{
    qDebug() << m_seq_state;
    return SEQ_T3;
}

t_seq_state Sequencer::STEP_T3()
{
    qDebug() << m_seq_state;
    return SEQ_T1;
}
