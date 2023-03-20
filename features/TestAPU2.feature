Feature: Tests APU design - 2

  Tests the APU design by validating files output by the simulation.
  Run the simulation for at least 0.5 seconds.
  VSMDD2 = TestDataAPU2.txt

  Scenario: Assembles test
    Given I run the command line: ..\c64\acme.exe -v4 --msvc "TestAPU2.a"
    Then property "test.BDD6502.lastProcessOutput" must contain string "Saving"
    


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
