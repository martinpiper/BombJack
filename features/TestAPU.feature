Feature: Tests APU design

  Tests the APU design by validating files output by the simulation.
  Run the simulation for at least 0.16 seconds.
  VSMDD2 = TestDataAPU1.txt

  Scenario: Assembles test
    Given I run the command line: ..\c64\acme.exe -v4 --msvc "TestAPU1.a"
    Then property "test.BDD6502.lastProcessOutput" must contain string "Saving"



  Scenario: Validates expected memory accesses for internal APU instruction memory
    Given open file "output\DebugAPUOutput.txt" for reading
    And skip line
    And skip line
    And skip line
    Then expect the next line to contain "d$00000000"
    And skip line
    And skip line
    And skip line
    Then expect the next line to contain "d$20000200"
    And skip line
    And skip line
    And skip line
    Then expect the next line to contain "d$20000201"
    And skip line
    And skip line
    And skip line
    Then expect the next line to contain "d$80000200"



  Scenario: Validates expected memory accesses for internal APU data memory
    Given open file "output\DebugAPUOutputInternal.txt" for reading
    And skip line
    Then expect the next line to contain "d$02080000"
    Then expect the next line to contain "delta:0.000003"
    Then expect the next line to contain "d$02090010"
    Then expect the next line to contain "delta:0.000002"
    Then expect the next line to contain "d$020a0002"
    Then expect the next line to contain "delta:0.000002"
    Then expect the next line to contain "d$020b0008"

    Then expect the next line to contain "delta:0.000008"
    Then expect the next line to contain "d$02080008"
    Then expect the next line to contain "delta:0.000002"
    Then expect the next line to contain "d$02090002"
    Then expect the next line to contain "delta:0.000002"
    Then expect the next line to contain "d$020a0010"
    Then expect the next line to contain "delta:0.000002"
    Then expect the next line to contain "d$020b0000"


    Then expect the next line to contain "delta:0.000486"
    Then expect the next line to contain "d$02080000"
    Then expect the next line to contain "delta:0.000002"
    Then expect the next line to contain "d$02090010"
    Then expect the next line to contain "delta:0.000002"
    Then expect the next line to contain "d$020a0002"
    Then expect the next line to contain "delta:0.000002"
    Then expect the next line to contain "d$020b0008"
    Then expect the next line to contain "delta:0.000006"
    Then expect the next line to contain "d$02080008"
    Then expect the next line to contain "delta:0.000002"
    Then expect the next line to contain "d$02090002"
    Then expect the next line to contain "delta:0.000002"
    Then expect the next line to contain "d$020a0010"
    Then expect the next line to contain "delta:0.000002"
    Then expect the next line to contain "d$020b0000"



  Scenario: Validates expected external memory writes
    Given open file "output\DebugAPUOutputJust9800.txt" for reading
    And skip line
    Then expect the next line to contain "d$98200111"
    And skip line
    Then expect the next line to contain "d$98210112"

    # External write during APU data memory write
    Then expect the next line to contain "delta:0.000014"
    Then expect the next line to contain "d$98200101"
    Then expect the next line to contain "delta:0.000002"
    Then expect the next line to contain "d$98210101"

    Then expect the next line to contain "delta:0.00002"
    Then expect the next line to contain "d$98200111"
    Then expect the next line to contain "delta:0.00046"
    Then expect the next line to contain "d$98210112"
    Then expect the next line to contain "delta:0.00001"
    Then expect the next line to contain "d$98200101"
    Then expect the next line to contain "delta:0.000001"
    Then expect the next line to contain "d$98210101"

    Then expect the next line to contain "delta:0.03301"
    Then expect the next line to contain "d$98200111"
    Then expect the next line to contain "delta:0.0005"
    Then expect the next line to contain "d$98210112"
    Then expect the next line to contain "delta:0.000010"
    Then expect the next line to contain "d$98200101"
    Then expect the next line to contain "delta:0.000001"
    Then expect the next line to contain "d$98210101"
    Then expect the next line to contain "delta:0.00002"
    Then expect the next line to contain "d$98200111"
    Then expect the next line to contain "delta:0.0162"
    Then expect the next line to contain "d$98210112"
    Then expect the next line to contain "delta:0.00001"
    Then expect the next line to contain "d$98200101"
    Then expect the next line to contain "delta:0.000001"
    Then expect the next line to contain "d$98210101"

    Then expect the next line to contain "delta:0.00002"
    Then expect the next line to contain "d$98200111"
    Then expect the next line to contain "delta:0.00047"
    Then expect the next line to contain "d$98210112"
    Then expect the next line to contain "delta:0.00001"
    Then expect the next line to contain "d$98200101"
    Then expect the next line to contain "delta:0.000001"
    Then expect the next line to contain "d$98210101"

    Then expect the next line to contain "delta:0.00002"
    Then expect the next line to contain "d$98200111"
    Then expect the next line to contain "delta:0.0162"
    Then expect the next line to contain "d$98210112"
    Then expect the next line to contain "delta:0.00001"
    Then expect the next line to contain "d$98200101"
    Then expect the next line to contain "delta:0.000001"
    Then expect the next line to contain "d$98210101"

    # Last kAPU_SkipIfEQ test enable
    # APUPC = 0x5b
    Then expect the next line to contain "delta:0.0005"
    Then expect the next line to contain "d$98000110"
    Then expect the next line to contain "delta:0.000001"
    Then expect the next line to contain "d$98010110"
    Then expect the next line to contain "delta:0.000001"
    Then expect the next line to contain "d$98020110"
    Then expect the next line to contain "delta:0.000002"
    Then expect the next line to contain "d$98030110"
    Then expect the next line to contain "delta:0.000001"
    Then expect the next line to contain "d$98040110"
    Then expect the next line to contain "delta:0.000001"
    Then expect the next line to contain "d$98050110"
    Then expect the next line to contain "delta:0.000001"
    Then expect the next line to contain "d$98060110"
    Then expect the next line to contain "delta:0.000001"
    Then expect the next line to contain "d$98070110"
    Then expect the next line to contain "delta:0.000001"
    Then expect the next line to contain "d$98080110"
    Then expect the next line to contain "delta:0.000001"
    Then expect the next line to contain "d$98090110"
    Then expect the next line to contain "delta:0.000002"
    Then expect the next line to contain "d$980a0110"







