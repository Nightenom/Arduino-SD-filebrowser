
#include <SPI.h>
#include <SD.h>

const String jobs[]={"mkdir","rmdir","readdir","mkfile","readfile","rmfile","openfile"};
const String edit[]={"append","closefile"};
boolean openFile=false;
File editor;

void setup() {
  Serial.begin(9600);
  while (!Serial) {}
  Serial.print("Initializing SD card...");

  if (!SD.begin()) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  Serial.println("Help: mkdir/rmdir/readdir/mkfile/openfile/closefile/readfile/rmfile [target];");
}

void loop() {
  if(Serial.available()) DoJob();
}

void DoJob() {
  File file;
  String job = Serial.readStringUntil(';');
  String target = job.substring(job.indexOf(" ")+1);
  int i;
  if(openFile) {
    for(i=0; i<2; i++) {
      if(edit[i]==job.substring(0,job.indexOf(" "))) break;
    }
    switch(i) {
      case 0: //append
        Serial.print("\tAppending: ");
        Serial.println(target);
        editor.print(target);
        break;
      case 1: //closefile
        Serial.println("Closing editor...");
        editor.close();
        openFile=false;
        break;
    }
    return;
  }
  for(i=0; i<7; i++) {
    if(jobs[i]==job.substring(0,job.indexOf(" "))) break;
  }
  if(i==7) {
    Serial.print("Job \"");
    Serial.print(job.substring(0,job.indexOf(" ")));
    Serial.println("\" does not exists");
    return;
  }
  if(!SD.exists(target) && target!="/" && i!=0 && i!=3) {
    Serial.print("File or directory \"");
    Serial.print(target);
    Serial.println("\" does not exists");
    return;
  }
  switch(i) {
    case 0: //mkdir
      Serial.print("Making directory \"");
      Serial.print(target);
      Serial.println("\"...");
      SD.mkdir(target);
      break;
    case 1: //rmdir
      Serial.print("Removing directory \"");
      Serial.print(target);
      Serial.println("\"...");
      SD.rmdir(target);
      break;
    case 2: //readdir
      Serial.print("Reading directory \"");
      Serial.print(target);
      Serial.println("\":");
      PrintDirectory(target);
      break;
    case 3: //makefile
      Serial.print("Making file \"");
      Serial.print(target);
      Serial.println("\"...");
      file = SD.open(target,FILE_WRITE);
      file.close();
      break;
    case 4: //readfile
      Serial.print("Reading file \"");
      Serial.print(target);
      Serial.println("\":");
      file = SD.open(target,FILE_READ);
      while(file.available()) Serial.write(file.read());
      file.close();
      Serial.println();
      break;
    case 5: //remfile
      Serial.print("Removing file \"");
      Serial.print(target);
      Serial.println("\"...");
      SD.remove(job.substring(job.indexOf(" ")+1));
      break;
    case 6: //openfile
      openFile=true;
      Serial.print("Opening editor \"");
      Serial.print(target);
      Serial.println("\":");
      editor = SD.open(target,FILE_WRITE);
      return;
    default:
      Serial.println("not working yet");
      break;
  }
  Serial.println("Done!");
}

void PrintDirectory(String dirr) {
  File dir = SD.open(dirr);
  while (true) {
    File entry =  dir.openNextFile();
    if(!entry) break; // no more files
    Serial.print('\t');
    Serial.print(entry.name());
    if(entry.isDirectory()) {
      Serial.print("\t");
      Serial.println("DIR");
    }
    else { // files have sizes, directories do not
      Serial.print("\t");
      Serial.print("FILE");
      Serial.print("\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
  dir.close();
}



