@APU
Feature: Tests APU design - 2 single pass

  Tests the APU design by validating files output by the simulation.
  This tests complex internal and external memory bus interactions that happen at the same time.
  Run the simulation for at least 0.1 seconds.


  @run
  Scenario: Assembles test and runs the simulation with single pass
    Given I run the command line: ..\c64\acme.exe -v4 --msvc "TestAPU2.a"
    Then property "test.BDD6502.lastProcessOutput" must contain string "Saving"

    Then I create file "ModelConfig_VSMDD2.txt" with
    """
    PATTERN=TestDataAPU2SinglePass.txt
    DELETETHISFILE=1
    """
    Then I create file "ModelConfig_VSMDD1.txt" with
    """
    EXITPROCCESSAFTER=0.1
    DELETETHISFILE=1
    """
    Then I create file "ModelConfig_VSMDD6.txt" with
    """
    FORCEFLUSH=1
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
    When run the APU simulation


  @check
  Scenario: Validates memory accesses contain the expected data after a single pass
    # Truncate the file to only after the APU is initialised
    When processing each line in file "testdata/toMatchTestAPU2.txt" and only output to file "target/out3DebugAPUOutput.txt" lines after finding a line containing "d$4105020f"
    When processing each line in file "target\out3DebugAPUOutput.txt" and only output to file "target/out3aDebugAPUOutput.txt" lines that do not contain any lines from "output\DebugAPUOutputJust9800.txt"
    # Indicating there is no missing expected data
    Given open file "target/out3aDebugAPUOutput.txt" for reading
    Then expect end of file
