#include <Arduino.h>
#include <iostream>
#include <string>
#include <regex>

bool isValidEtmSerial(std::string str) {
  if (str.length() != 14 || std::regex_search(str.c_str(), std::regex("[^A-Z0-9]"))) {
    return false;
  }

  int num = 0;
  for (int i2 = 0; i2 < 11; ++i2) {
    try {
      num += (std::stoi(str.substr(i2, i2 + 1))) * (i2 + 1);
    } catch (const std::exception& unused) {
      num = 0;
      break;
    }
  }

  int i;
  try {
    i = std::stoi(str.substr(11, 14));
  } catch (const std::exception& unused2) {
    i = 0;
  }

  return num > 0 && num % 256 == i;
}

bool isValidMobiusSerial(std::string str) {
  if (str.length() != 14 || std::regex_search(str, std::regex("[^A-Z0-9]"))) {
    return false;
  }

  int i = 0;
  for (char c : str.substr(0, 12)) {
    i += static_cast<unsigned short>(c);
  }

  int num;
  try {
    num = std::stoi(str.substr(12, 2), nullptr, 16);
  } catch (std::exception& e) {
    num = 0;
  }

  return num == i % 256;
}

bool isValidSerial(std::string str) {
    return isValidMobiusSerial(str) || isValidEtmSerial(str);
}

/* All Above is working */

char* getEtmModelInfo(short s) {
	if (s == 1) {
		return "VorTech MP10w ES";
	}
	if (s == 21) {
		return "VorTech MP10w ES";
	}
	if (s == 31) {
		return "VorTech MP10w ES";
	}
	if (s == 3) {
		return "VorTech MP40w ES";
	}
	if (s == 4) {
		return "VorTech MP60w ES";
	}
	if (s == 7) {
		return "Radion XR30w";
	}
	if (s == 8) {
		return "Radion XR30w G2";
	}
	if (s == 9) {
		return "Radion XR30w Pro";
	}
	if (s == 23) {
		return "VorTech MP40w ES";
	}
	if (s == 24) {
		return "VorTech MP60w ES";
	}
	if (s == 33) {
		return "VorTech MP40w ES";
	}
	if (s != 34) {
		switch (s) {
			case 27:
				return "Radion XR30w";
			case 28:
				return "Radion XR30w G2";
			case 29:
				return "Radion XR30w Pro";
			default:
				switch (s) {
					case 37:
						return "Radion XR30w";
					case 38:
						return "Radion XR30w G2";
					case 39:
						return "Radion XR30w Pro";
					case 40:
						return "Vectra S1";
					case 41:
						return "Vectra S1";
					case 42:
						return "Vectra S2";
					case 43:
						return "Vectra S2";
					case 44:
						return "Vectra M2";
					case 45:
						return "Vectra M2";
					case 46:
						return "VectraL2";
					case 47:
						return "VectraL2";
					default:
						switch (s) {
							case 52:
								return "Radion XR30w G3";
							case 53:
								return "Radion XR30w G3";
							case 54:
								return "Radion XR30w G3";
							case 55:
								return "Radion XR30w G3 Pro";
							case 56:
								return "Radion XR30w G3 Pro";
							case 57:
								return "Radion XR30w G3 Pro";
							case 58:
								return "Radion XR15w G3 Pro";
							case 59:
								return "Radion XR15w G3 Pro";
							case 60:
								return "Radion XR15w G3 Pro";
							case 61:
								return "Radion XR15w FW";
							case 62:
								return "Radion XR15w FW";
							case 63:
								return "Radion XR15w FW";
							case 64:
								return "Radion XR30w G3";
							case 65:
								return "Radion XR30w G3";
							case 66:
								return "Radion XR30w G3";
							case 67:
								return "Radion XR30w G3 Pro";
							case 68:
								return "Radion XR30w G3 Pro";
							case 69:
								return "Radion XR30w G3 Pro";
							case 70:
								return "VorTech MP10w QD";
							case 71:
								return "VorTech MP10w QD";
							case 72:
								return "VorTech MP10w QD";
							case 73:
								return "VorTech MP10w QD";
							case 74:
								return "VorTech MP10w QD";
							case 75:
								return "VorTech MP10w QD";
							case 76:
								return "VorTech MP40w QD";
							case 77:
								return "VorTech MP40w QD";
							case 78:
								return "VorTech MP40w QD";
							case 79:
								return "VorTech MP60w QD";
							case 80:
								return "VorTech MP60w QD";
							case 81:
								return "VorTech MP60w QD";
							case 82:
								return "Vectra M1";
							case 83:
								return "Vectra M1";
							case 84:
								return "VectraL1";
							case 85:
								return "VectraL1";
							case 86:
								return "Radion XR15w G4 Pro";
							case 87:
								return "Radion XR15w G4 Pro";
							case 88:
								return "Radion XR15w G4 Pro";
							case 89:
								return "Radion XR30w G4";
							case 90:
								return "Radion XR30w G4";
							case 91:
								return "Radion XR30w G4";
							case 92:
								return "Radion XR30w G4 Pro";
							case 93:
								return "Radion XR30w G4 Pro";
							case 94:
								return "Radion XR30w G4 Pro";
							case 95:
								return "Radion XR15w FW Pro";
							case 96:
								return "Radion XR15w FW Pro";
							case 97:
								return "Radion XR30w FW Pro";
							case 98:
								return "Radion XR30w FW Pro";
							default:
								return nullptr;
						}
				}
		}
	}
	return "VorTech MP60w ES";
}

#include <string>
#include <cstdlib>

char* Model(short modelNum) {
  switch (modelNum) {
  case 0:
    return "Unknown";
    break;
  case 10:
    return "VorTech MP10wES";
    break;
  case 11:
    return "VorTech MP10wQD";
    break;
  case 30:
    return "Radion XR30w";
    break;
  case 31:
    return "Radion XR30w G2";
    break;
  case 32:
    return "Radion XR30w Pro";
    break;
  case 33:
    return "Radion XR30w G3";
    break;
  case 34:
    return "Radion XR30w G3 Pro";
    break;
  case 35:
    return "Radion XR30w G4";
    break;
  case 36:
    return "Radion XR30w G4 Pro";
    break;
  case 37:
    return "Radion XR30w FW Pro";
    break;
  case 39:
    return "Radion XR30w Prototype";
    break;
  case 40:
    return "VorTech MP40w ES";
    break;
  case 41:
    return "VorTech MP40w QD";
    break;
  case 42:
    return "VorTech MP40w G3 QD";
    break;
  case 60:
    return "VorTech MP60w ES";
    break;
  case 61:
    return "VorTech MP60w QD";
    break;
  case 144:
    return "Vectra M1";
    break;
  case 145:
    return "VectraL1";
    break;
  case 146:
    return "Vectra S1";
    break;
  case 147:
    return "Vectra S2";
    break;
  case 148:
    return "Vectra M2";
    break;
  case 149:
    return "VectraL2";
    break;
  case 160:
    return "Radion XR15w G3 Pro";
    break;
  case 161:
    return "Radion XR15wFW";
    break;
  case 162:
    return "Radion XR15w G4 Pro";
    break;
  case 163:
    return "Radion XR15w FW Pro";
    break;
  case 175:
    return "Radion XR15w Prototype";
    break;
  case 176:
    return "Radion XR15 G5 Pro";
    break;
  case 177:
    return "Radion XR15w G5 Blue";
    break;
  case 178:
    return "BeefChalupa";
    break;
  case 179:
    return "SteakQuesadilla";
    break;
  case 180:
    return "SteakNachoFriesBurrito"; // Replace with the actual value
    break; 
  case 192:
    return "Radion XR30w G5 Pro";
    break;
  case 193:
    return "Radion XR30w G5 Blue";
    break;
  case 194:
    return "CrunchwrapSupreme";
    break;
  case 195:
    return "SteakTaco";
    break;
  case 196:
    return "BeefyMeltBurrito";
    break;
  case 256:
    return "Nero 5";
    break;
  case 257:
    return "Nero 3";
    break;
  case 258:
    return "Slack";
    break;
  case 259:
    return "Alpaca2";
    break;
  case 260:
    return "Alpaca4";
    break;
  case 261:
    return "Turtle1";
    break;
  case 262:
    return "Turtle2";
    break;
  case 263:
    return "Turtle3";
    break;
  case 264:
    return "Turtle4";
    break;
  case 272:
    return "Versa VX-1";
    break;
  case 275:
    return "Muffler";
    break;
  case 288:
    return "Chalupa";
    break;
  case 304:
    return "DoritosLocosTacos";
    break;
  case 320:
    return "Prime";
    break;
  case 321:
    return "Prime HD";
    break;
  case 322:
    return "Prime FW";
    break;
  case 323:
    return "Hydra 26 HD";
    break;
  case 324:
    return "Hydra 52 HD";
    break;
  case 325:
    return "RedSea MAX13";
    break;
  case 326:
    return "RedSea MAX26";
    break;
  case 333:
    return "Prime 16 HD";
    break;
  case 334:
    return "Hydra 32 HD";
    break;
  case 335:
    return "Hydra 64 HD";
    break;
  case 339:
    return "Prime 16 HdFw";
    break;
  case 340:
    return "Prime 16 HdFuge";
    break;
  case 341:
    return "Prime 16 HdSol";
    break;
  case 342:
    return "Prime Fuge";
    break;
  case 344:
    return "Quesarito";
    break;
  case 345:
    return "NachosBellGrande";
    break;
  case 346:
    return "Plumbus";
    break;
  case 347:
    return "Saddle";
    break;
  case 349:
    return "BeanBurrito";
    break;
  case 350:
    return "Cowboy";
  default:
    return "Unknown";
    break;
  }
};

char* parseMobiusSerialModel(std::string str){
  short parsedValue = std::stoi(str.substr(2, 2), nullptr, 36);
  return Model(parsedValue); 
}

char* parseEtmSerialModel(std::string str) {
  short value = std::atoi(str.substr(0, 2).c_str());
  return getEtmModelInfo(value);
}

char* getModelName(std::string mySerial){
//  Serial.print("Serial #: ");
//  Serial.print(mySerial.c_str());

  if (isValidSerial(mySerial)) {
    if (isValidMobiusSerial(mySerial)){
      return parseMobiusSerialModel(mySerial);
    }
    else if (isValidEtmSerial(mySerial)){
      return parseEtmSerialModel(mySerial);      
    }
    else{
      return "Unknown";
    }
  } else {
    return "Invalid";
  }

}

