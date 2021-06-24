#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <QObject>

typedef enum
{
    SEQ_T1,
    SEQ_T2,
    SEQ_T3,
    SEQ_T4,
    SEQ_T5,
    SEQ_T6,
    SEQ_T7
}t_seq_state;

class Sequencer: public QObject
{
    Q_OBJECT

    signals:
        void getData(const QByteArray &data);

    public:
        explicit Sequencer(QObject *parent = nullptr);
        ~Sequencer();

        t_seq_state getStep();
        void update();
        t_seq_state STEP_T1();
        t_seq_state STEP_T2();
        t_seq_state STEP_T3();



    private:
        t_seq_state m_seq_state;

};

#endif // SEQUENCER_H
