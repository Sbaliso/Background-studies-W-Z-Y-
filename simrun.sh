#!/bin/bash

function runcommand(){
    echo -e "\n"
    echo -e "\n" >&2

    echo "* $1 : $2"
    echo "* $1 : $2" >&2

    time aliroot -b -q -x $2 >>$3 2>&1
    exitcode=$?

    expectedCode=${5-0}

    if [ "$exitcode" -ne "$expectedCode" ]; then
        echo "*! $2 failed with exitcode $exitcode, expecting $expectedCode"
        echo "*! $2 failed with exitcode $exitcode, expecting $expectedCode" >&2
        echo "$2 failed with exitcode $exitcode, expecting $expectedCode" > validation_error.message
        exit ${4-$exitcode}
    else
        echo "* $2 finished with the expected exit code ($expectedCode), moving on"
        echo "* $2 finished with the expected exit code ($expectedCode), moving on" >&2
    fi
}

# Define the pt hard bin arrays
pthardbin_loweredges=( 0 5 11 21 36 57 84 117 152 191 234 )
pthardbin_higheredges=( 5 11 21 36 57 84 117 152 191 234 -1)

CONFIG_SEED=""
CONFIG_RUN_TYPE=""
CONFIG_FIELD=""
CONFIG_ENERGY=""
CONFIG_PHYSICSLIST=""
CONFIG_BMIN=""
CONFIG_BMAX=""
CONFIG_PTHARDBIN=""
CONFIG_PTHARDMIN=""
CONFIG_PTHARDMAX=""
CONFIG_QUENCHING=""
DC_RUN=""
DC_EVENT=""
EVENTS_PER_JOB=""
TRANSPORT_CODE=""
BEAMCONFIG=""
COLLISIONSYSTEM=""
GENERATOR=""

RUNMODE=""

while [ ! -z "$1" ]; do
    option="$1"
    shift

    if [ "$option" = "--run" ]; then
            DC_RUN="$1"
            shift
    elif [ "$option" = "--event" ]; then
            DC_EVENT="$1"
            shift
    elif [ "$option" = "--process" ]; then
            CONFIG_RUN_TYPE="$1"
            shift
    elif [ "$option" = "--field" ]; then
            CONFIG_FIELD="$1"
            shift
    elif [ "$option" = "--energy" ]; then
            CONFIG_ENERGY="$1"
            shift
    elif [ "$option" = "--physicslist" ]; then
            CONFIG_PHYSICSLIST="$1"
            shift
    elif [ "$option" = "--bmin" ]; then
            CONFIG_BMIN="$1"
            shift
    elif [ "$option" = "--bmax" ]; then
            CONFIG_BMAX="$1"
            shift
    elif [ "$option" = "--pthardbin" ]; then
            CONFIG_PTHARDBIN="$1"
            shift
    elif [ "$option" = "--quench" ]; then
            CONFIG_QUENCHING="$1"
            shift
    elif [ "$option" = "--eventsPerJob" ]; then
            EVENTS_PER_JOB="$1"
            shift
    elif [ "$option" = "--transport" ]; then
            TRANSPORT_CODE="$1"
            shift
    elif [ "$option" = "--generator" ]; then
            GENERATOR="$1"
            shift
    elif [ "$option" = "--collisionSystem" ]; then
            COLLISIONSYSTEM="$1"
            shift
    elif [ "$option" = "--beamConfig" ]; then
            BEAMCONFIG="$1"
            shift

    fi
done

CONFIG_SEED=$((ALIEN_PROC_ID%1000000000))

if [ "$CONFIG_SEED" -eq 0 ]; then
    CONFIG_SEED=$(((DC_RUN*100000+DC_EVENT)%1000000000))
    echo "* MC Seed is $CONFIG_SEED (based on run / counter : $DC_RUN / $DC_EVENT)"
else
    echo "* MC Seed is $CONFIG_SEED (based on AliEn job ID)"
fi

if [ "$CONFIG_SEED" -eq 0 ]; then
    echo "*!  WARNING! Seeding variable for MC is 0 !" >&2
fi

echo "* b min is $CONFIG_BMIN"
echo "* b max is $CONFIG_BMAX"
echo "* pt hard bin is $CONFIG_PTHARDBIN"

if [ ! -z "$CONFIG_PTHARDBIN" ]; then
    # Define environmental vars for pt binning
    CONFIG_PTHARDMIN=${pthardbin_loweredges[$CONFIG_PTHARDBIN]}
    CONFIG_PTHARDMAX=${pthardbin_higheredges[$CONFIG_PTHARDBIN]}

    echo "* pt hard from $CONFIG_PTHARDMIN to $CONFIG_PTHARDMAX"
fi

mkdir input
mv galice.root ./input/galice.root
mv Kinematics.root ./input/Kinematics.root
ls input

export CONFIG_SEED CONFIG_RUN_TYPE CONFIG_FIELD CONFIG_ENERGY CONFIG_PHYSICSLIST CONFIG_BMIN CONFIG_BMAX CONFIG_PTHARDBIN CONFIG_PTHARDMIN CONFIG_PTHARDMAX DC_RUN DC_EVENT BEAMCONFIG COLLISIONSYSTEM TRANSPORT_CODE GENERATOR

export ALIMDC_RAWDB1="./mdc1"
export ALIMDC_RAWDB2="./mdc2"
export ALIMDC_TAGDB="./mdc1/tag"
export ALIMDC_RUNDB="./mdc1/meta"

if [ -f "$G4INSTALL/bin/geant4.sh" ]; then
    echo "* Sourcing G4 environment from $G4INSTALL/bin/geant4.sh"
    source $G4INSTALL/bin/geant4.sh
fi

echo "SIMRUN:: Run $DC_RUN Event $DC_EVENT Generator $CONFIG_RUN_TYPE Field $CONFIG_FIELD Energy $CONFIG_ENERGY Physicslist $CONFIG_PHYSICSLIST"

BEAM1=""
BEAM2=""
HADRON1=""
HADRON2=""
if [ "$GENERATOR" = "powheg" ]; then
  echo  "  >>> Run POWHEG <<<"
  nevts=1000
  if [ ! -z $EVENTS_PER_JOB ]; then
    nevts=$EVENTS_PER_JOB
  fi
  if [ "$BEAMCONFIG" = "pPb"  ]; then
      BEAM1=208
      BEAM2=1
      if [ "$COLLISIONSYSTEM" = "pp"  ]; then
          HADRON1=1
          HADRON2=1
      else
          HADRON1=2
          HADRON2=1
      fi
  else
    BEAM1=1
    BEAM2=208
    if [ "$COLLISIONSYSTEM" = "pp"  ]; then
        HADRON1=1
        HADRON2=1
    else
        HADRON1=1
        HADRON2=2
    fi
  fi
  nevts=$(expr $nevts \* 80)
  awk -v lseed="$CONFIG_SEED" -v lnumevts="$nevts" -v lbeam1="$BEAM1" -v lbeam2="$BEAM2" -v lh1="$HADRON1" -v lh2="$HADRON2" '{
   if ( index($0,"iseed") ) gsub("12345",lseed);
   if ( index($0,"numevts") ) gsub("12345",lnumevts);
   if ( index($0,"AA1") ) gsub("zz",lbeam1);
   if ( index($0,"AA2") ) gsub("tt",lbeam2);
   if ( index($0,"ih1") ) gsub("xx",lh1);
   if ( index($0,"ih2") ) gsub("yy",lh2);
   print $0; }' base_powheg.input > powheg.input

  pwhg_main_W> powheg.log 2>&1
fi

simCommand="sim.C"
if [ ! -z $EVENTS_PER_JOB ]; then
  simCommand="sim.C($EVENTS_PER_JOB)"
fi

runcommand "SIMULATION" "$simCommand" sim.log 5
mv syswatch.log simwatch.log

runcommand "RECONSTRUCTION" "rec.C" rec.log 10
mv syswatch.log recwatch.log

#runcommand "TAG" "tag.C" tag.log 50

runcommand "CHECK ESD" "CheckESD.C" check.log 60 1

runcommand "AODtrainsim" "AODtrainsim.C" AODTrain.log

#rm -f *.RecPoints.root

#if [ "$RUNMODE" = "SDD" ]; then
#    if [ -f QAtrainsim_SDD.C ]; then
#            runcommand "Running the QA train" "QAtrainsim_SDD.C(\"_wSDD\",$DC_RUN)" qa.log 100

#            for file in *.stat; do
#                mv $file ../$file.qa_wSDD
#            done
#    fi

#    mv AliESDs.root AliESDs_wSDD.root
#    mv AliESDfriends.root AliESDfriends_wSDD.root

    # Without SDD

#    for logfile in rec.log qa.log tag.log check.log recwatch.log; do
#            echo -e "\n\n* ------------ Without SDD ------------" >> $logfile
#    done

#    runcommand "RECONSTRUCTION without SDD" "recNoSDD.C" rec.log 11
#    cat syswatch.log >> recwatch.log
#    rm syswatch.log

#    runcommand "TAG without SDD" "tag.C" tag.log 51

#    runcommand "CHECK ESD without SDD" "CheckESD.C" check.log 61 1

#    if [ -f QAtrainsim_SDD.C ]; then
#            runcommand "Running the QA train without SDD" "QAtrainsim_SDD.C(\"\",$DC_RUN)" qa.log 101

#            for file in *.stat; do
#                mv $file ../$file.qa
#            done
#    fi
#fi

rm -f *.RecPoints.root *.Hits.root *.Digits.root *.SDigits.root

exit 0
