Feature: Tests UserPortTo24BitAddress design

  Tests the UserPortTo24BitAddress design by validating files output by the simulation.
  Run the simulation for at least 0.5 seconds.

  Scenario: Validates expected memory accesses for internal APU instruction memory
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

