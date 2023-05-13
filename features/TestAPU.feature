Feature: Tests APU design

  Tests the APU design by validating files output by the simulation.
  Run the simulation for at least 0.16 seconds.
  VSMDD2 = TestDataAPU1.txt

  Scenario: Assembles test and runs the simulation
    Given I run the command line: ..\c64\acme.exe -v4 --msvc "TestAPU1.a"
    Then property "test.BDD6502.lastProcessOutput" must contain string "Saving"

    Then I create file "ModelConfig_VSMDD2.txt" with
    """
    PATTERN=TestDataAPU1.txt
    DELETETHISFILE=1
    """
    # The recording model should be the one to terminate the simulation, so it can flush its data properly
    Then I create file "ModelConfig_VSMDD6.txt" with
    """
    EXITPROCCESSAFTER=0.16
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
    Then expect the next line to contain "delta:0.000004"
    Then expect the next line to contain "d$02090010"
    Then expect the next line to contain "delta:0.000004"
    Then expect the next line to contain "d$020a0002"
    Then expect the next line to contain "delta:0.000004"
    Then expect the next line to contain "d$020b0008"

    Then expect the next line to contain "delta:0.000011"
    Then expect the next line to contain "d$02080008"
    Then expect the next line to contain "delta:0.000002"
    Then expect the next line to contain "d$02090002"
    Then expect the next line to contain "delta:0.000002"
    Then expect the next line to contain "d$020a0010"
    Then expect the next line to contain "delta:0.000002"
    Then expect the next line to contain "d$020b0000"


    Then expect the next line to contain "delta:0.0004"
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
    Then expect the next line to contain "delta:0.000022"
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
    Then expect the next line to contain "delta:0.0004"
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
    Then expect the next line to contain "delta:0.000002"
    Then expect the next line to contain "d$98090110"
    Then expect the next line to contain "delta:0.000001"
    Then expect the next line to contain "d$980a0110"




