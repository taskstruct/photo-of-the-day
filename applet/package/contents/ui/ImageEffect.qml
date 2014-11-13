import QtQuick 2.0

ShaderEffect {
    id: effect
    
    blending: false
    
    property bool flip: false
    property real amount: 0.0
    property alias mask: maskSrc.sourceItem
    property alias running: transition.running
    
    property url frontSource: ""
    property real aspectRatio
    
    onFrontSourceChanged: { 
//         console.debug( "onFrontSourceChanged " + frontSource )
        
        if( effect.flip )
        {
//             console.debug("Connecting buff1")
            connection.target = buff1
            buff1.source = frontSource
        }
        else
        {
//             console.debug("Connecting buff2")
            connection.target = buff2
            buff2.source = frontSource
        }
    }
    
    Behavior on amount {
        enabled: rootItem.animatedTransitionsCfg
        NumberAnimation { duration: rootItem.__animationsDuration * 10 }
    }
    
    Image {
        id: buff1
    }
    
    Image {
        id: buff2
    }
    
    ShaderEffectSource {
        id: maskSrc
        smooth: true
        hideSource: true
        sourceItem: mask
    }
    
    ShaderEffectSource {
        id: frontImageSrc
        hideSource: true
        sourceItem: buff1
    }
    
    ShaderEffectSource {
        id: backImageSrc
        hideSource: true
        sourceItem: buff2
    }
    
    Connections {
        id: connection
        
        target: null
        
        onStatusChanged: if (target.status == Image.Ready) {
            effect.aspectRatio = target.sourceSize.width / target.sourceSize.height
            
//             console.debug("Connections onStatusChanged target.source" + target.source )
//             console.debug("Connections onStatusChanged buff1.source" + buff1.buff1 )
//             console.debug("Connections onStatusChanged buff2.source" + buff2.source )
            
            effect.flip = !effect.flip
        }
    }
    
    property variant fImg: frontImageSrc
    property variant bImg: backImageSrc
    property variant mImg: maskSrc
    
    fragmentShader: "
        varying highp vec2 qt_TexCoord0;
        uniform sampler2D fImg;
        uniform sampler2D bImg;
        uniform sampler2D mImg;
        uniform lowp float amount;
        uniform lowp float qt_Opacity;
        void main()
        {
            lowp vec4 texm = texture2D(mImg, qt_TexCoord0);
            
            if( texm.a == 0.0 )
            {
                // under border or outside rounded corner
                gl_FragColor = vec4( 0.0, 0.0, 0.0, 0.0 );
            }
            else
            {
                if( amount == 0.0 ) 
                {
                    gl_FragColor = vec4( texture2D(fImg, qt_TexCoord0).rgb, texm.a) * qt_Opacity;
                }
                else
                {
                    lowp vec4 tex1 = texture2D(fImg, qt_TexCoord0);
                    lowp vec4 tex2 = texture2D(bImg, qt_TexCoord0);
                    
                    gl_FragColor = vec4( (tex1.rgb * (1.0 - amount) + tex2.rgb * amount ) , texm.a) * qt_Opacity;
                }
            }
    }"
    
    states: State {
        name: "flipped"
        when: effect.flip
        
        PropertyChanges { target: effect; amount: 1.0 }
    }
    
    transitions: Transition {
        id: transition
        from: ""
        to: "flipped"
        enabled: animatedTransitionsCfg
        reversible: true
        
        NumberAnimation {
            target: effect
            property: "amount"
            duration: __animationsDuration
        }
    }
}