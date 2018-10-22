/*
 * Tarpeeksi Hyvae Soft 2018 /
 * AV Scissors.
 *
 * Loads in sound data from WAV files.
 *
 * Is fairly strict about what kind of WAV it will accept; but anything produced as FFMPEG
 * does with the options "-flags bitexact -map_metadata -1 -acodec pcm_s16le -ac 1" should
 * work.
 *
 */

#include <QByteArray>
#include <QDebug>
#include <QFile>
#include "../../src/messager/messager.h"
#include "../../src/audio/audio_file.h"
#include "../../src/common.h"

audio_file_c::audio_file_c(const QString audioFilename, const messager_c *const messager) :
    filename(audioFilename)
{
    connect(    this, &audio_file_c::message_to_user,
            messager, &messager_c::new_message);

    this->extract_audio_data();

    return;
}

audio_file_c::~audio_file_c()
{
    if (this->waveform != nullptr)
    {
        delete [] this->waveform;
    }

    return;
}

// Expects a WAV file.
//
void audio_file_c::extract_audio_data(void)
{
    QFile file(this->filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        emit message_to_user("Could not read the audio data.");
        NBENE(("Failed to open audio file '%s'", this->filename.toLatin1().constData()));
        return;
    }

    QByteArray fileData = file.readAll();
    if (fileData.isEmpty())
    {
        emit message_to_user("Could not read the audio data.");
        NBENE(("Failed to load audio data from '%s'", this->filename.toLatin1().constData()));
        return;
    }
    file.close();

    // Parse the data.
    /// TODO: Add some more checks and verification, e.g. that the RIFF chunk is
    /// as we expect it to.
    {
        QDataStream dataStream(fileData);

        // Riff chunk.
        {
            if ((dataStream.skipRawData(4) == -1) ||    // Should be "RIFF".
                (dataStream.skipRawData(4) == -1) ||    // Should be chunk size.
                (dataStream.skipRawData(4) == -1))      // Should be "WAVE".
            {
                NBENE(("Failed when reading the audio RIFF chunk."));
                emit message_to_user("Could not read the audio data.");
                return;
            }
        }

        // Fmt chunk.
        {
            if ((dataStream.skipRawData(4) == -1) ||    // Should be "fmt ".
                (dataStream.skipRawData(4) == -1) ||    // Should be chunk size.
                (dataStream.skipRawData(2) == -1) ||    // Should be audio format.
                (dataStream.readRawData((char*)&this->numChannels, 2) == -1) ||
                (dataStream.readRawData((char*)&this->sampleRate, 4) == -1) ||
                (dataStream.skipRawData(4) == -1) ||    // Should be byte rate.
                (dataStream.readRawData((char*)&this->blockAlign, 2) == -1) ||
                (dataStream.readRawData((char*)&this->bitsPerSample, 2) == -1))
            {
                NBENE(("Failed when reading the audio fmt chunk."));
                emit message_to_user("Could not read the audio data.");
                return;
            }

            // Assert some hard-coded assumptions we have about how the audio should be.
            if ((this->numChannels != 1) ||     // Expect a mono audio file.
                (this->bitsPerSample != 16))    // Expect 16-bit audio samples.
            {
                emit message_to_user("Could not read the audio data.");
                return;
            }
        }

        // Data chunk.
        {
            if ((dataStream.skipRawData(4) == -1) ||    // Should be "data".
                (dataStream.readRawData((char*)&this->numSamples, 4) == -1))
            {
                NBENE(("Failed when reading the audio data chunk."));
                emit message_to_user("Could not read the audio data.");
                return;
            }

            // The actual sample data.
            {
                this->numSamples /= this->blockAlign;
                this->waveform = new short[this->numSamples];

                if (dataStream.readRawData((char*)this->waveform, (this->numSamples * this->blockAlign)) == -1)
                {
                    NBENE(("Failed when reading the audio samples."));
                    emit message_to_user("Could not read the audio data.");

                    delete [] this->waveform;
                    this->waveform = nullptr;

                    return;
                }
            }
        }
    }

    this->durationMs = ((this->numSamples / (real)this->sampleRate) * 1000.0);

    return;
}
