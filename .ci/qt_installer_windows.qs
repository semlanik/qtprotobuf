function Controller() {
    installer.autoRejectMessageBoxes();
    installer.installationFinished.connect(function() {
        gui.clickButton(buttons.NextButton);
    })
}

Controller.prototype.WelcomePageCallback = function() {
    gui.clickButton(buttons.NextButton, 5000);
}

Controller.prototype.CredentialsPageCallback = function() {
    gui.currentPageWidget().loginWidget.EmailLineEdit.setText("ci@semlanik.org");
    gui.currentPageWidget().loginWidget.PasswordLineEdit.setText("1QazxsW2");
    gui.clickButton(buttons.NextButton, 5000);
}

Controller.prototype.IntroductionPageCallback = function() {
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.TargetDirectoryPageCallback = function() {
    gui.currentPageWidget().TargetDirectoryLineEdit.setText("C:/Qt");
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.DynamicTelemetryPluginFormCallback = function() {
    var page = gui.pageWidgetByObjectName("DynamicTelemetryPluginForm");
    page.statisticGroupBox.disableStatisticRadioButton.setChecked(true);
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.ComponentSelectionPageCallback = function() {
    var widget = gui.currentPageWidget();
    widget.deselectAll();
    widget.selectComponent("qt.qt5.5152.win32_msvc2019");

    gui.clickButton(buttons.NextButton);
}

Controller.prototype.ObligationsPageCallback = function()
{
    var page = gui.pageWidgetByObjectName("ObligationsPage");
    page.obligationsAgreement.setChecked(true);
    page.completeChanged();
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.LicenseAgreementPageCallback = function() {
    gui.currentPageWidget().AcceptLicenseRadioButton.setChecked(true);
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.StartMenuDirectoryPageCallback = function() {
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.ReadyForInstallationPageCallback = function()
{
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.FinishedPageCallback = function() {
var checkBoxForm = gui.currentPageWidget().LaunchQtCreatorCheckBoxForm
if (checkBoxForm && checkBoxForm.launchQtCreatorCheckBox) {
    checkBoxForm.launchQtCreatorCheckBox.checked = false;
}
    gui.clickButton(buttons.FinishButton);
} 
