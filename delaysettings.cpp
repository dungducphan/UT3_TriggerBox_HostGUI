#include "delaysettings.h"

DelaySettings::DelaySettings() {
    for (unsigned int i =0; i < 6; i++) m_DelayInMilliseconds.push_back(90);
}

DelaySettings::~DelaySettings() = default;

unsigned int DelaySettings::GetDelay(unsigned int ID) const {
    return m_DelayInMilliseconds.at(ID);
}

void DelaySettings::SetDelay(unsigned int ID, unsigned int delay) {
    m_DelayInMilliseconds.at(ID) = delay;
}
