Feature: Tests APU design

  Tests the APU design by validating files output by the simulation.

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


    Then expect the next line to contain "delta:0.000479"
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
    Then expect the next line to contain "delta:0.015968"
    Then expect the next line to contain "d$98210112"
    Then expect the next line to contain "delta:0.000010"
    Then expect the next line to contain "d$98200101"
    Then expect the next line to contain "delta:0.000001"
    Then expect the next line to contain "d$98210101"

    Then expect the next line to contain "delta:0.000022"
    Then expect the next line to contain "d$98200111"
    Then expect the next line to contain "delta:0.000461"
    Then expect the next line to contain "d$98210112"
    Then expect the next line to contain "delta:0.000010"
    Then expect the next line to contain "d$98200101"
    Then expect the next line to contain "delta:0.000001"
    Then expect the next line to contain "d$98210101"

    Then expect the next line to contain "delta:0.033012"
    Then expect the next line to contain "d$98200111"
    Then expect the next line to contain "delta:0.000500"
    Then expect the next line to contain "d$98210112"
    Then expect the next line to contain "delta:0.000010"
    Then expect the next line to contain "d$98200101"
    Then expect the next line to contain "delta:0.000001"
    Then expect the next line to contain "d$98210101"
    Then expect the next line to contain "delta:0.000022"
    Then expect the next line to contain "d$98200111"
    Then expect the next line to contain "delta:0.016220"
    Then expect the next line to contain "d$98210112"
    Then expect the next line to contain "delta:0.000010"
    Then expect the next line to contain "d$98200101"
    Then expect the next line to contain "delta:0.000001"
    Then expect the next line to contain "d$98210101"
