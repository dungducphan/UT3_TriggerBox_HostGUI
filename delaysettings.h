#ifndef DELAYSETTINGS_H
#define DELAYSETTINGS_H

#include <vector>

class DelaySettings {
public:
    DelaySettings();
    ~DelaySettings();

    unsigned int GetDelay(unsigned int ID) const;
    void SetDelay(unsigned int ID, unsigned int delay);

private:
    std::vector<unsigned int> m_DelayInMilliseconds;
};

#endif // DELAYSETTINGS_H
