#include <httplib.h>

#include <cstdint>

enum class Mode {
  COOL, ECO, FAN
};

enum class FanMode {
  AUTO, HIGH, MEDIUM, LOW
};

class AC {
private:

  Mode mode;
  FanMode fanMode;
  unsigned int temp{};
  bool sleeping{};
  bool powered{};

  static constexpr uint8_t tempsCelsius[17*8] = {0,0,0,0, 0, 0,0, 0,
                                1,0,0,0, 0, 0,0, 0,
                                0,1,0,0, 0, 0,0, 0,
                                1,1,0,0, 0, 0,0, 0,
                                0,0,1,0, 0, 0,0, 0,
                                1,0,1,0, 0, 0,0, 0,
                                0,1,1,0, 0, 0,0, 0,
                                1,1,1,0, 0, 0,0, 0,
                                0,0,0,1, 0, 0,0, 0,
                                1,0,0,1, 0, 0,0, 0,
                                0,1,0,1, 0, 0,0, 0,
                                1,1,0,1, 0, 0,0, 0,
                                0,0,1,1, 0, 0,0, 0,
                                1,0,1,1, 0, 0,0, 0,
                                0,1,1,1, 0, 0,0, 0,
                                0,1,1,1, 0, 1,0, 1,
                                0,1,1,1, 0, 0,1, 1};

  static constexpr uint8_t coolCelsiusCodes[17*4] = {1,1,0,1,
                                    0,0,1,1,
                                    1,0,1,1,
                                    0,1,1,1,
                                    1,1,1,1,
                                    0,0,0,0,
                                    1,0,0,0,
                                    0,1,0,0,
                                    1,1,0,0,
                                    0,0,1,0,
                                    1,0,1,0,
                                    0,1,1,0,
                                    1,1,1,0,
                                    0,0,0,1,
                                    1,0,0,1,
                                    1,0,0,1,
                                    1,0,0,1};

  static constexpr uint8_t ecoCelsiusCodes[17*4] = {1,1,1,1,
                                   0,0,0,0,
                                   1,0,0,0,
                                   0,1,0,0,
                                   1,1,0,0,
                                   0,0,1,0,
                                   1,0,1,0,
                                   0,1,1,0,
                                   1,1,1,0,
                                   0,0,0,1,
                                   1,0,0,1,
                                   0,1,0,1,
                                   1,1,0,1,
                                   0,0,1,1,
                                   1,0,1,1,
                                   1,0,1,1,
                                   1,0,1,1};

  static constexpr uint8_t fanCelsiusCodes[17*4] = {1,0,1,1,
                                   0,1,1,1,
                                   1,1,1,1,
                                   0,0,0,0,
                                   1,0,0,0,
                                   0,1,0,0,
                                   1,1,0,0,
                                   0,0,1,0,
                                   1,0,1,0,
                                   0,1,1,0,
                                   1,1,1,0,
                                   0,0,0,1,
                                   1,0,0,1,
                                   0,1,0,1,
                                   1,1,0,1,
                                   1,1,0,1,
                                   1,1,0,1};

  static constexpr uint8_t fanFahrenheitCodes[31*4] = {1,1,1,0,
                                      1,1,0,0,
                                      0,0,1,0,
                                      0,0,0,1,
                                      1,0,1,0,
                                      1,0,0,1,
                                      0,1,1,0,
                                      0,1,0,1,
                                      1,1,1,0,
                                      0,0,0,1,
                                      0,0,1,1,
                                      1,0,0,1,
                                      1,0,1,1,
                                      0,1,0,1,
                                      0,1,1,1,
                                      1,1,0,1,
                                      1,1,1,1,
                                      0,0,1,1,
                                      1,0,1,1,
                                      1,0,0,0,
                                      0,1,1,1,
                                      0,1,0,0,
                                      1,1,1,1,
                                      1,1,0,0,
                                      0,0,0,0,
                                      0,0,1,0,
                                      1,0,0,0,
                                      1,0,0,0,
                                      1,0,1,0,
                                      1,0,0,0,
                                      1,0,1,0};

  static constexpr uint8_t coolFahrenheitCodes[31*4] = {1,1,1,0,
                                       1,1,0,0,
                                       0,0,1,0,
                                       0,0,0,1,
                                       1,0,1,0,
                                       1,0,0,1,
                                       0,1,1,0,
                                       0,1,0,1,
                                       1,1,1,0,
                                       0,0,0,1,
                                       0,0,1,1,
                                       1,0,0,1,
                                       1,0,1,1,
                                       0,1,0,1,
                                       0,1,1,1,
                                       1,1,0,1,
                                       1,1,1,1,
                                       0,0,1,1,
                                       1,0,1,1,
                                       1,0,0,0,
                                       0,1,1,1,
                                       0,1,0,0,
                                       1,1,1,1,
                                       1,1,0,0,
                                       0,0,0,0,
                                       0,0,1,0,
                                       1,0,0,0,
                                       1,0,0,0,
                                       1,0,1,0,
                                       1,0,0,0,
                                       1,0,1,0};

  static constexpr uint8_t ecoFahrenheitCodes[31*4] = {1,1,0,1,
                                      1,1,1,0,
                                      0,0,0,1,
                                      0,0,1,1,
                                      1,0,0,1,
                                      1,0,1,1,
                                      0,1,0,1,
                                      0,1,1,1,
                                      1,1,0,1,
                                      0,0,1,1,
                                      0,0,0,0,
                                      1,0,1,1,
                                      1,0,0,0, //
                                      0,1,1,1,
                                      0,1,0,0,
                                      1,1,1,1,
                                      1,1,0,0,
                                      0,0,0,0,
                                      1,0,0,0,
                                      1,0,1,0,
                                      0,1,0,0,
                                      0,1,1,0,
                                      1,1,0,0,
                                      1,1,1,0,
                                      0,0,1,0,
                                      0,0,0,1,
                                      1,0,1,0,
                                      1,0,1,0,
                                      1,0,0,1,
                                      1,0,1,0,
                                      1,0,0,1};

  static constexpr uint8_t tempsFahrenheit[31*8] = {0,0,0,0,1,0,0,1,
                                   0,0,0,0,0,0,0,0,
                                   1,0,0,0,0,0,0,0,
                                   1,0,0,0,1,0,0,0,
                                   0,1,0,0,0,0,0,0,
                                   0,1,0,0,1,0,0,0,
                                   1,1,0,0,0,0,0,0,
                                   1,1,0,0,1,0,0,0,
                                   0,0,1,0,0,0,0,0,
                                   1,0,1,0,0,0,0,0,
                                   1,0,1,0,1,0,0,0,
                                   0,1,1,0,0,0,0,0,
                                   0,1,1,0,1,0,0,0,
                                   1,1,1,0,0,0,0,0,
                                   1,1,1,0,1,0,0,0,
                                   0,0,0,1,0,0,0,0,
                                   0,0,0,1,1,0,0,0,
                                   1,0,0,1,0,0,0,0,
                                   0,1,0,1,0,0,0,0,
                                   0,1,0,1,1,0,0,0,
                                   1,1,0,1,0,0,0,0,
                                   1,1,0,1,1,0,0,0,
                                   0,0,1,1,0,0,0,0,
                                   0,0,1,1,1,0,0,0,
                                   1,0,1,1,0,0,0,0,
                                   1,0,1,1,1,0,0,0,
                                   0,1,1,1,0,0,0,0,
                                   0,1,1,1,0,1,0,1,
                                   0,1,1,1,1,1,0,1,
                                   0,1,1,1,0,0,1,1,
                                   0,1,1,1,1,0,1,1};
public:

  [[nodiscard]] bool isSleeping() const {
    return sleeping;
  }

  AC() {
    setSleeping(false);
    setMode(Mode::ECO);
    setTemp(22);
    setPowered(true);
    setFanMode(FanMode::LOW);
  }

  void setSleeping(const bool _sleeping) {
    this->sleeping = _sleeping;
    rawData[7] = sleeping ? 1 : 0;
  }

  void setTemp(const unsigned int _temp) {
    bool celsius;
    if(_temp <= 90 && _temp >= 60)
      celsius = false;
    else if(_temp <= 32 && _temp >= 16)
      celsius = true;
    else
      return;
    this->temp = _temp;
    const uint8_t* tempData;
    const unsigned int tempOffset = celsius ? (temp - 16) : (temp - 60);
    if(celsius)
      tempData = tempsCelsius + 8 * tempOffset;
    else
      tempData = tempsFahrenheit + 8 * tempOffset;

    rawData[8] = tempData[0];
    rawData[9] = tempData[1];
    rawData[10] = tempData[2];
    rawData[11] = tempData[3];

    rawData[26] = tempData[4];
    rawData[27] = (celsius ? 0 : 1);

    const uint8_t* code;
    if(mode == Mode::COOL)
      if(celsius)
        code = coolCelsiusCodes + 4 * tempOffset;
      else
        code = coolFahrenheitCodes + 4 * tempOffset;
    else if(mode == Mode::ECO) {
      if(celsius)
        code = ecoCelsiusCodes + 4 * tempOffset;
      else
        code = ecoFahrenheitCodes + 4 * tempOffset;
    } else if(mode == Mode::FAN) {
      if(celsius)
        code = fanCelsiusCodes + 4 * tempOffset;
      else
        code = fanFahrenheitCodes + 4 * tempOffset;
    } else {
      throw std::runtime_error("Cannot set temperature with an invalid mode.");
    }

    rawData[63] = code[0];
    rawData[64] = code[1];
    rawData[65] = code[2];
    rawData[66] = code[3];

    rawData[87] = tempData[5];
    rawData[88] = tempData[6];

    rawData[90] = tempData[7];
  }

  void setFanMode(const FanMode _fanMode) {
    this->fanMode = _fanMode;
    if(fanMode == FanMode::AUTO) {
      rawData[4] = 0;
      rawData[5] = 0;
    } else if(fanMode == FanMode::HIGH) {
      rawData[4] = 1;
      rawData[5] = 1;
    } else if(fanMode == FanMode::MEDIUM) {
      rawData[4] = 0;
      rawData[5] = 1;
    } else if(fanMode == FanMode::LOW) {
      rawData[4] = 1;
      rawData[5] = 0;
    }
  }

  void setMode(const Mode _mode) {
    this->mode = _mode;
    if(mode == Mode::COOL) {
      rawData[0] = 1;
      rawData[1] = 0;
      rawData[2] = 0;
      setTemp(temp);
    } else if(mode == Mode::ECO) {
      rawData[0] = 1;
      rawData[1] = 0;
      rawData[2] = 1;
      setTemp(temp);
    } else if(mode == Mode::FAN) {
      rawData[0] = 1;
      rawData[1] = 1;
      rawData[2] = 0;
      setTemp(temp);
    }
  }

  void setTimer() {
    rawData[15] = 1; // turns it on
  }

  void setPowered(const bool _powered) {
    this->powered = _powered;
    if(powered) {
      rawData[3] = 1;
      rawData[11] = 0;
      rawData[22] = 1;
    } else {
      rawData[3] = 0;
      rawData[11] = 1;
      rawData[22] = 0;
    }
  }

  [[nodiscard]] std::string getState() const {
    std::string state(134, '0');
    for(std::size_t i = 0; const auto bit : rawData)
      state[i++] = bit ? '1' : '0';
    return state;
  }

  [[nodiscard]] bool isPowered() const {
    return powered;
  }

  [[nodiscard]] Mode getMode() const {
    return mode;
  }

  [[nodiscard]] FanMode getFanMode() const {
    return fanMode;
  }

  [[nodiscard]] unsigned int getTemp() const {
    return temp;
  }

  uint8_t rawData[134] = {1,0,1,1,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,1,0,1,0,0,1,0,
                          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,
                          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,
                          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1};
};

void sendState(const AC& ac) {
  httplib::Client cli("192.168.0.66");
  cli.set_connection_timeout(30);
  auto res = cli.Post("/send/", ac.getState(), "text/plain");
}

int main() {
  AC ac;
  ac.setMode(Mode::FAN);
  ac.setFanMode(FanMode::HIGH);
  sendState(ac);

  httplib::Server server;

  server.Get("/api/listen-temps", [&](const httplib::Request &request, httplib::Response &res) {
    res.set_content("", "text/plain");
    const int humidity = std::stoi(request.get_param_value("hum"));
    const float temp = std::stof(request.get_param_value("temp"));
    const std::string id = request.get_param_value("id");
    std::cout << humidity << " " << temp << " " << id << " ";

    if(temp <= 21 && (ac.getMode() != Mode::FAN)) {
      ac.setMode(Mode::FAN);
      sendState(ac);
      std::cout << "turned ac off";
    } else if(temp >= 23 && (ac.getMode() != Mode::COOL)) {
      ac.setMode(Mode::COOL);
      ac.setTemp(20);
      sendState(ac);
      std::cout << "turned ac on";
    }
    std::cout << std::endl;

  });

  server.listen("192.168.0.33", 80);

  return 0;
}
