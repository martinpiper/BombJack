Feature: Tests APU design - 4

  Tests the APU design by validating files output by the simulation.
  Run the simulation for at least 0.1 seconds.
  VSMDD2 = TestDataAPU4.txt
  Mirrors JUnit checkAPU9

  Scenario: Assembles test and runs the simulation
    Given I run the command line: ..\c64\acme.exe -v4 --msvc "TestAPU4.a"
    Then property "test.BDD6502.lastProcessOutput" must contain string "Saving"

    Then I create file "ModelConfig_VSMDD2.txt" with
    """
    PATTERN=TestDataAPU4.txt
    DELETETHISFILE=1
    """
    # The recording model should be the one to terminate the simulation, so it can flush its data properly
    Then I create file "ModelConfig_VSMDD6.txt" with
    """
    EXITPROCCESSAFTER=0.1
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



  Scenario: Validates expected external memory writes
    Given open file "output\DebugAPUOutputJust9800.txt" for reading
    And skip line
    Then expect the next line to contain "d$980001a5"
    Then expect the next line to contain "delta:0.000001"
    Then expect the next line to contain "d$980101e1"
    Then expect the next line to contain "delta:0.000001"
    Then expect the next line to contain "d$98020116"
    Then expect the next line to contain "delta:0.000001"
    Then expect the next line to contain "d$980301a5"
    Then expect the next line to contain "delta:0.000001"
    Then expect the next line to contain "d$9804016f"
    Then expect the next line to contain "delta:0.000001"
    Then expect the next line to contain "d$980501db"
    Then expect the next line to contain "delta:0.004670"
    Then expect the next line to contain "d$980001a5"
    Then expect the next line to contain "delta:0.000001"
    Then expect the next line to contain "d$980101e1"

