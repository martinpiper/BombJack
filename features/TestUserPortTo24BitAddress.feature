Feature: Tests UserPortTo24BitAddress design

  Tests the UserPortTo24BitAddress design by validating files output by the simulation.
  Run the simulation for at least 0.5 seconds.

  Scenario: Validates expected memory accesses for internal APU instruction memory

    Given I create file "ModelConfig_VSMDD2.txt" with
    """
    PATTERN=TestUserPortTo24BitAddress1.txt
    EXITPROCCESSAFTER=0.5
    DELETETHISFILE=1
    """

    Given starting an automation process "cmd" with parameters: /c UserPortTo24BitAddress2.pdsprj
    When automation find window from pattern ".*UserPortTo24BitAddress2.*Proteus.*"
    When automation focus window
    When automation expand main menu item "Debug"
    When automation click current menu item "Run Simulation.*F12"
    Then automation wait for window close


    Given open file "output\DebugUserPortTo24BitAddress.txt" for reading
    Then expect the next line to contain "d$00000000"
    Then expect the next line to contain "d$02100101"
    Then expect the next line to contain "d$02110102"
    Then expect the next line to contain "d$02120182"
    Then expect the next line to contain "d$02130181"
    Then expect the next line to contain "d$01240111"
    Then expect the next line to contain "d$01250122"
    Then expect the next line to contain "d$01260132"
    Then expect the next line to contain "d$01270141"

