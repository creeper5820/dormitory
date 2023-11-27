#include "NfcController.hh"

int count_loop;
NfcController nfc;

bool flag_nfc_debug = 0;

void setup()
{
    nfc.Init();
    count_loop = 0;
    Serial.begin(115200);
}

void loop()
{
    if (count_loop % 5 == 0) {

        if (flag_nfc_debug)
            Serial.println("Read");

        if (nfc.Read()) {

            if (flag_nfc_debug)
                Serial.println("Receice");
        }
    }

    if (count_loop % 50 == 0) {

        if (flag_nfc_debug)
            Serial.println("Find");

        nfc.Find();
    }

    if (count_loop % 100 == 0) {

        if (flag_nfc_debug)
            Serial.println("Awake");

        nfc.Awake();
        count_loop = 0;
    }

    delay(100);
    count_loop++;
}
