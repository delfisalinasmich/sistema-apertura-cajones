#ifndef PREFERENCES_MOCK_H
#define PREFERENCES_MOCK_H

// Mock mínimo de la librería Preferences (NVS en flash) que usa
// sensor_linea.h, como un simple key-value en memoria — alcanza para
// probar la máquina de estados sin una calibración real guardada (el test
// arranca siempre con cal.valida=false, umbral por defecto, ver
// sensor_linea.h). Ver test/README.md.

class Preferences {
public:
  void begin(const char* name, bool readOnly = false);
  void end();
  bool isKey(const char* key);
  int getInt(const char* key, int valorPorDefecto = 0);
  bool putInt(const char* key, int valor);
  bool getBool(const char* key, bool valorPorDefecto = false);
  bool putBool(const char* key, bool valor);
};

#endif
