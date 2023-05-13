Feature: Tests APU design - 2

  Tests the APU design by validating files output by the simulation.
  Run the simulation for at least 0.5 seconds.
  VSMDD2 = TestDataAPU2.txt

  Scenario: Assembles test and runs the simulation
    Given I run the command line: ..\c64\acme.exe -v4 --msvc "TestAPU2.a"
    Then property "test.BDD6502.lastProcessOutput" must contain string "Saving"

    Then I create file "ModelConfig_VSMDD2.txt" with
    """
    PATTERN=TestDataAPU2.txt
    DELETETHISFILE=1
    """
    # The recording model should be the one to terminate the simulation, so it can flush its data properly
    Then I create file "ModelConfig_VSMDD6.txt" with
    """
    EXITPROCCESSAFTER=0.5
    DELETETHISFILE=1
    """
    Then I create file "ModelConfig_VSMDD3.txt" with
    """
    FORCEFLUSH=1
    DELETETHISFILE=1
    """
    Then I create file "ModelConfig_VSMDD4.txt" with
    """
    FORCEFLUSH=1
    DELETETHISFILE=1
    """
    Then I create file "ModelConfig_VSMDD5.txt" with
    """
    FORCEFLUSH=1
    DELETETHISFILE=1
    """


    Given starting an automation process "cmd" with parameters "/c APU.pdsprj"
    When automation find window from pattern ".*APU.*Proteus.*"
    When automation focus window
    When automation expand main menu item "Debug"
    When automation click current menu item "Run Simulation.*F12"
    Then automation wait for window close



  Scenario: Validates expected memory accesses for internal APU instruction memory
    # Truncate the file to only after the APU is initialised
    When processing each line in file "output\DebugAPUOutput.txt" and only output to file "target/out2DebugAPUOutput.txt" lines after finding a line containing "d$20000203"
    When processing each line in file "target/out2DebugAPUOutput.txt" and only output to file "target/out2aDebugAPUOutput.txt" lines that do not contain any lines from "testdata/toMatchTestAPU2.txt"
    # Indicating there are no unexpected lines
    Given open file "target/out2aDebugAPUOutput.txt" for reading
    Then expect end of file



  Scenario: Validates expected external memory writes
    When processing each line in file "output\DebugAPUOutputJust9800.txt" and only output to file "target/out2aDebugAPUOutputJust9800.txt" lines that do not contain any lines from "testdata/toMatchTestAPU2.txt"
    Given open file "target/out2aDebugAPUOutputJust9800.txt" for reading
    Then expect end of file
