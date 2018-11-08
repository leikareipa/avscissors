/*
 * Tarpeeksi Hyvae Soft 2018 /
 * AV Scissors.
 *
 */

#ifndef AUDIO_FILE_H
#define AUDIO_FILE_H

#include <QObject>
#include <QString>
#include "../../src/common.h"

class messager_c;

class audio_file_c : public QObject
{
    Q_OBJECT

public:
    audio_file_c(const QString audioFilename, const messager_c *const messager);
    ~audio_file_c(void);

    uint num_samples(void) const
    {
        return this->numSamples;
    }

    uint sample_rate(void) const
    {
        return this->sampleRate;
    }

    uint duration_ms(void) const
    {
        return this->durationMs;
    }

    short sample_at(const uint offs) const
    {
        k_assert(offs < numSamples, "Accessing audio data out of bounds.");

        return waveform[offs];
    }

    bool has_valid_audio_data(void) const
    {
        return bool(waveform != nullptr);
    }

signals:
    void message_to_user(const QString message);

private:
    void extract_audio_data();

    const QString filename;

    // The raw audio samples. (Assumes them to be signed 16-bit.)
    short *waveform = nullptr;
    uint numSamples;

    // Properties of the audio stream.
    uint durationMs;
    short numChannels;
    uint sampleRate;
    short bitsPerSample;
    short blockAlign;
};

#endif
