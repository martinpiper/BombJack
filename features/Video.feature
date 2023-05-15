Feature: Tests the video hardware with expected output
  
  
  Scenario: Simple image comparison test

    Then I create file "ModelConfig_VSMDD2.txt" with
    """
    PATTERN=TestData.txt
    EXITPROCCESSAFTER=0.50
    DELETETHISFILE=1
    """

    Given starting an automation process "cmd" with parameters: /c BombJack.pdsprj
    When automation find window from pattern ".*BombJack.*Proteus.*"
    When automation focus window
    When automation expand main menu item "Debug"
    When automation click current menu item "Run Simulation.*F12"
    Then automation wait for window close

    Then expect image "testdata/debug00000000.bmp" to be identical to "output/debug00000000.bmp"
    Then expect image "testdata/debug00000001.bmp" to be identical to "output/debug00000001.bmp"
    Then expect image "testdata/debug00000002.bmp" to be identical to "output/debug00000002.bmp"
    Then expect image "testdata/debug00000003.bmp" to be identical to "output/debug00000003.bmp"
    Then expect image "testdata/debug00000004.bmp" to be identical to "output/debug00000004.bmp"
    Then expect image "testdata/debug00000005.bmp" to be identical to "output/debug00000005.bmp"
    Then expect image "testdata/debug00000006.bmp" to be identical to "output/debug00000006.bmp"
    Then expect image "testdata/debug00000007.bmp" to be identical to "output/debug00000007.bmp"
    Then expect image "testdata/debug00000008.bmp" to be identical to "output/debug00000008.bmp"
    Then expect image "testdata/debug00000009.bmp" to be identical to "output/debug00000009.bmp"
    Then expect image "testdata/debug00000010.bmp" to be identical to "output/debug00000010.bmp"
    Then expect image "testdata/debug00000011.bmp" to be identical to "output/debug00000011.bmp"
    Then expect image "testdata/debug00000012.bmp" to be identical to "output/debug00000012.bmp"
    Then expect image "testdata/debug00000013.bmp" to be identical to "output/debug00000013.bmp"
    Then expect image "testdata/debug00000014.bmp" to be identical to "output/debug00000014.bmp"
    Then expect image "testdata/debug00000015.bmp" to be identical to "output/debug00000015.bmp"
    Then expect image "testdata/debug00000016.bmp" to be identical to "output/debug00000016.bmp"
    Then expect image "testdata/debug00000017.bmp" to be identical to "output/debug00000017.bmp"
    Then expect image "testdata/debug00000018.bmp" to be identical to "output/debug00000018.bmp"
    Then expect image "testdata/debug00000019.bmp" to be identical to "output/debug00000019.bmp"
    Then expect image "testdata/debug00000020.bmp" to be identical to "output/debug00000020.bmp"
    Then expect image "testdata/debug00000021.bmp" to be identical to "output/debug00000021.bmp"
    Then expect image "testdata/debug00000022.bmp" to be identical to "output/debug00000022.bmp"
    Then expect image "testdata/debug00000023.bmp" to be identical to "output/debug00000023.bmp"
    Then expect image "testdata/debug00000024.bmp" to be identical to "output/debug00000024.bmp"
    Then expect image "testdata/debug00000025.bmp" to be identical to "output/debug00000025.bmp"
