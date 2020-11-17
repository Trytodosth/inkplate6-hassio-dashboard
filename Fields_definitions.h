/*
 * This file's only purpose is to keep the main .ino clean from
 * many lines for definitions.
 * Note the fields have unrealistic characteristic values to
 * recognize a problem in the data.
 */


// Variables for local forecasts
char today[3][24] = {
  "99.9",
  "-9.9",
  "exceptional",
};
char tomorr[3][24] = {
  "88.8",
  "-8.8",
  "exceptional",
};

// Variables for storing sensors' data
char sensor1[4][12] = {
  "Cuisine",
  "90.0",
  "9000",
  "90.0",
};
char sensor2[4][12] = {
  "Chambre",
  "80.0",
  "8000",
  "80.0",
};
char sensor3[4][12] = {
  "Grange",
  "70.0",
  "7000",
  "70.0",
};

// Variables for storing cities' data
char city1[4][24] = {
  "Lyon",
  "77.7",
  "-7.7",
  "exceptional",
};
char city2[4][24] = {
  "Suhr",
  "66.6",
  "-6.6",
  "exceptional",
};
char city3[4][24] = {
  "Chambly",
  "55.5",
  "-5.5",
  "exceptional",
};
