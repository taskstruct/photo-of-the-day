import QtQuick 2.0

ShaderEffect {
    id: effect
    
    property real amount: 0.0

    property alias frontImage: frontImageEffectSrc.sourceItem
    property alias backImage: backImageEffectSrc.sourceItem


    property variant _frontImage: ShaderEffectSource {
        id: frontImageEffectSrc
    }

    property variant _backImage: ShaderEffectSource {
        id: backImageEffectSrc
    }

    property variant _maskRect: Rectangle {
        id: maskRectangle

        color: "white"

        anchors.fill: parent
    }

    property alias maskRadius: maskRectangle.radius
    
    fragmentShader: "
        varying highp vec2 qt_TexCoord0;
        uniform sampler2D _frontImage;
        uniform sampler2D _backImage;
        uniform sampler2D _maskRect;
        uniform lowp float amount;
        uniform lowp float qt_Opacity;
        void main()
        {
            lowp vec4 maskTex = texture2D(_maskRect, qt_TexCoord0);
            lowp float maskOp = maskTex.a;

            if( amount == 0.0 )
            {
                gl_FragColor = texture2D(_frontImage, qt_TexCoord0.st) * maskOp * qt_Opacity;
            }
            else
            {
                lowp vec4 tex1 = texture2D(_frontImage, qt_TexCoord0.st);
                lowp vec4 tex2 = texture2D(_backImage, qt_TexCoord0.st);
                
                gl_FragColor = ( tex1 * amount + tex2 * ( 1.0 - amount ) ) * maskOp * qt_Opacity;
            }
    }"
}
