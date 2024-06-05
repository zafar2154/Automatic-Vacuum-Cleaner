#include <DHT.h>
#include <Fuzzy.h>

#define DHTPIN 8     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 22 (AM2302)

DHT dht(DHTPIN, DHTTYPE);

float dhtread(){
  return dht.readHumidity(); 
}

#define TRIG_PIN 11
#define ECHO_PIN 10
float ultraread(){
  long duration;

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  return (duration / 2) / 29.1; 
}

#define MOTOR_IN1_PIN 7
#define MOTOR_IN2_PIN 6
#define MOTOR_ENA_PIN 5

// Fuzzy
Fuzzy *fuzzy = new Fuzzy();

// FuzzyInput Jarak
FuzzySet *near = new FuzzySet(0, 0, 5, 10);
FuzzySet *sedang = new FuzzySet(5, 20, 20, 30);
FuzzySet *far = new FuzzySet(20, 30, 50, 50);

// FuzzyInput Kelembaban
FuzzySet *kering = new FuzzySet(0, 0, 0, 50);
FuzzySet *agaklembab = new FuzzySet(20, 60, 70, 90);
FuzzySet *lembab = new FuzzySet(80, 100, 120, 150);

FuzzySet *die = new FuzzySet(0, 0, 0, 0);
FuzzySet *pelan = new FuzzySet(0, 0, 50, 150);
FuzzySet *fast = new FuzzySet(100, 150, 150, 230);
FuzzySet *veryfast = new FuzzySet(220 , 255, 255, 255);
void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(MOTOR_ENA_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(MOTOR_IN1_PIN, OUTPUT);
  pinMode(MOTOR_IN2_PIN, OUTPUT);

  FuzzyInput *jarak = new FuzzyInput(1);
  jarak->addFuzzySet(sedang);
  jarak->addFuzzySet(near);
  jarak->addFuzzySet(far);
  fuzzy->addFuzzyInput(jarak);

  FuzzyInput *humidityInput = new FuzzyInput(2);

  humidityInput->addFuzzySet(kering);
  humidityInput->addFuzzySet(agaklembab);
  humidityInput->addFuzzySet(lembab);
  fuzzy->addFuzzyInput(humidityInput);

  FuzzyOutput *motorspeed = new FuzzyOutput(1);
  motorspeed->addFuzzySet(die);
  motorspeed->addFuzzySet(pelan);
  motorspeed->addFuzzySet(fast);
  motorspeed->addFuzzySet(veryfast);
  fuzzy->addFuzzyOutput(motorspeed);

//fuzzyrule 1
  FuzzyRuleAntecedent *dekatkering = new FuzzyRuleAntecedent();
  dekatkering->joinWithAND(near, kering);
  FuzzyRuleConsequent *fast1 = new FuzzyRuleConsequent();
  fast1->addOutput(fast);
  FuzzyRule *FuzzyRule1 = new FuzzyRule(1, dekatkering, fast1);
  fuzzy->addFuzzyRule(FuzzyRule1);
  
//fuzzyrule 2
  FuzzyRuleAntecedent *dekatagaklembab = new FuzzyRuleAntecedent();
  dekatagaklembab->joinWithAND(near, agaklembab);
  FuzzyRuleConsequent *vf = new FuzzyRuleConsequent();
  vf->addOutput(veryfast);
  FuzzyRule *FuzzyRule2 = new FuzzyRule(2, dekatagaklembab, vf);
  fuzzy->addFuzzyRule(FuzzyRule2);

//fuzzyrule 3
  FuzzyRuleAntecedent *dekatlembab = new FuzzyRuleAntecedent();
  dekatlembab->joinWithAND(near, lembab);
  FuzzyRuleConsequent *slower = new FuzzyRuleConsequent();
  slower->addOutput(pelan);
  FuzzyRule *FuzzyRule3 = new FuzzyRule(3, dekatlembab, slower);
  fuzzy->addFuzzyRule(FuzzyRule3);

//fuzzyrule 4
  FuzzyRuleAntecedent *jauhkering = new FuzzyRuleAntecedent();
  jauhkering->joinWithAND(far, kering);
  FuzzyRuleConsequent *die1 = new FuzzyRuleConsequent();
  die1->addOutput(die);
  FuzzyRule *FuzzyRule4 = new FuzzyRule(4, jauhkering, die1);
  fuzzy->addFuzzyRule(FuzzyRule4);

//fuzzyrule 5
  FuzzyRuleAntecedent *jauhagaklembab = new FuzzyRuleAntecedent();
  jauhagaklembab->joinWithAND(far, agaklembab);
  FuzzyRuleConsequent *die2 = new FuzzyRuleConsequent();
  die2->addOutput(die);
  FuzzyRule *FuzzyRule5 = new FuzzyRule(5, jauhagaklembab, die2);
  fuzzy->addFuzzyRule(FuzzyRule5);

//fuzzyrule 6
  FuzzyRuleAntecedent *jauhlembab = new FuzzyRuleAntecedent();
  jauhlembab->joinWithAND(far, lembab);
  FuzzyRuleConsequent *die3 = new FuzzyRuleConsequent();
  die3->addOutput(die);
  FuzzyRule *FuzzyRule6 = new FuzzyRule(6, jauhlembab, die3);
  fuzzy->addFuzzyRule(FuzzyRule6);

  FuzzyRuleAntecedent *sl = new FuzzyRuleAntecedent();
  sl->joinWithAND(sedang, lembab);
  FuzzyRuleConsequent *slower1 = new FuzzyRuleConsequent();
  slower1->addOutput(pelan);
  FuzzyRule *FuzzyRule7 = new FuzzyRule(7, sl, slower1);
  fuzzy->addFuzzyRule(FuzzyRule7);

  FuzzyRuleAntecedent *sk = new FuzzyRuleAntecedent();
  sk->joinWithAND(sedang, kering);
  FuzzyRuleConsequent *slower2 = new FuzzyRuleConsequent();
  slower2->addOutput(pelan);
  FuzzyRule *FuzzyRule8 = new FuzzyRule(8, sk, slower2);
  fuzzy->addFuzzyRule(FuzzyRule8);

  FuzzyRuleAntecedent *sal = new FuzzyRuleAntecedent();
  sal->joinWithAND(sedang, agaklembab);
  FuzzyRuleConsequent *slower3 = new FuzzyRuleConsequent();
  slower3->addOutput(pelan);
  FuzzyRule *FuzzyRule9 = new FuzzyRule(9, sal, slower3);
  fuzzy->addFuzzyRule(FuzzyRule9);
}

void loop() {
  delay(1000);
  float humidity=dhtread();
  float distance=ultraread();
  float rlembab=random(1,100);
  float rjarak=random(1,50);
  
  fuzzy->setInput(1, distance);
  fuzzy->setInput(2, humidity);
  fuzzy->fuzzify();

  float velocity = fuzzy->defuzzify(1);
  analogWrite(MOTOR_ENA_PIN, velocity);

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Print temperature, humidity, and distance to the Serial Monitor
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("%\t");
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  Serial.print("\tDistance: Near-> ");
  Serial.print(near->getPertinence());

  Serial.print(", middle-> ");
  Serial.print(sedang->getPertinence());

  Serial.print(", far-> ");
  Serial.print(far->getPertinence());

  Serial.print("\tHumidity: kering-> ");
  Serial.print(kering->getPertinence());
  Serial.print(", agak lembab-> ");
  Serial.print(agaklembab->getPertinence());
  Serial.print(", lembab-> ");
  Serial.println(lembab->getPertinence());

  Serial.print("\tSpeed: die-> ");
  Serial.print(die->getPertinence());
  Serial.print(",  slow-> ");
  Serial.print(pelan->getPertinence());  
  Serial.print(",  fast-> ");
  Serial.print(fast->getPertinence());
  Serial.print(",  very fast-> ");
  Serial.println(veryfast->getPertinence());
  digitalWrite(MOTOR_IN1_PIN, LOW);
  digitalWrite(MOTOR_IN2_PIN, HIGH);

  Serial.println("Result : ");
  Serial.print("      Kelembaban : ");
  if(humidity<=35){
    Serial.println("kering");
  } else if(humidity>35 && humidity<=85) {
    Serial.println("agak lembab");
  } else if(humidity>85) {
    Serial.println("lembab");
  }
  Serial.print("      jarak : ");
  if(distance<=8){
    Serial.println("dekat");
  } else if(distance>8 && distance<=32){
    Serial.println("sedang");
  } else if(distance>32){
    Serial.println("jauh");
  }
  Serial.print("      kecepatan : ");
  Serial.println(velocity);
  Serial.print("\n\n");
}
