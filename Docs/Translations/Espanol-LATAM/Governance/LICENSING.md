# ¿Por qué EUPL 1.2?

[Português (Brasil)](../../Portugues-Brasil/Governance/LICENSING.md) · [English](../../../Governance/LICENSING.md)

Cosmeron está licenciada bajo la **Licencia Pública de la Unión Europea, Versión 1.2 solamente (EUPL-1.2)**.

Este documento explica por qué Cosmeron eligió la EUPL. Es únicamente informativo y no modifica, reemplaza ni complementa los términos de [LICENSE](../../../../LICENSE).

## ¿Por qué EUPL?

### Una licencia para el proyecto

La EUPL 1.2 se aplica a la "Obra", es decir, a la Obra Original o a sus Obras Derivadas, y la licencia trata expresamente esa Obra como una obra o software, en lugar de limitar su alcance únicamente al software ejecutable.[^1]

Esto la hace adecuada para el código fuente de Cosmeron y para otros materiales protegidos por derechos de autor distribuidos como parte del proyecto, como la documentación y el material técnico. La orientación oficial sobre la EUPL también describe la licencia como aplicable al software y a datos auxiliares.[^2]

### Reciprocidad sin aislar la biblioteca

Cosmeron está pensada para ser útil como biblioteca dentro de proyectos más grandes, incluso proyectos distribuidos bajo distintos modelos de licenciamiento.

La EUPL fue concebida teniendo en cuenta la interoperabilidad. La orientación oficial sobre la EUPL explica que las interfaces, APIs y estructuras de datos pueden reutilizarse para implementar enlace estático o dinámico con componentes independientes sin cambiar automáticamente la licencia de esos componentes independientes.[^2]

Esa orientación se apoya, en particular, en la **Directiva 2009/24/CE sobre la protección jurídica de programas de ordenador**. El considerando 10 describe la interoperabilidad como la capacidad de los componentes de software y hardware para intercambiar información y utilizar mutuamente la información intercambiada, mientras que el considerando 15 y el artículo 6 establecen una excepción limitada de interoperabilidad cuando la reproducción o traducción del código resulta indispensable para obtener la información necesaria para que un programa creado de forma independiente sea interoperable con otros programas.[^3]

La orientación sobre compatibilidad de la EUPL parte de esas disposiciones al tratar el linking. También señala que no existe jurisprudencia específica del Tribunal de Justicia que resuelva definitivamente la cuestión del linking, por lo que Cosmeron presenta esto como interpretación y orientación oficial de la EUPL, y no como una decisión judicial.[^4]

Al mismo tiempo, cuando las modificaciones u obras derivadas cubiertas por la EUPL se distribuyen o comunican al público, el artículo 5 preserva obligaciones recíprocas sobre la Obra cubierta y exige que el código fuente se proporcione o se mantenga disponible libremente.[^1]

En términos prácticos, el objetivo del proyecto es:

    Usá Cosmeron libremente.
    Integrá Cosmeron libremente.
    Mejorá Cosmeron libremente.
    Compartí las mejoras distribuidas de Cosmeron.

### Protección de las mejoras compartidas

La naturaleza recíproca de la EUPL ayuda a mantener las mejoras distribuidas y las obras derivadas de la Obra cubierta disponibles bajo términos abiertos. El artículo 5 exige que la Distribución o Comunicación de la Obra cubierta o de Obras Derivadas permanezca bajo la EUPL, salvo cuando corresponda aplicar el mecanismo de compatibilidad.[^1]

Para Cosmeron, esto favorece una base técnica compartida sin impedir que la biblioteca forme parte de sistemas más grandes y con licencias diferentes.

### Compatibilidad

La EUPL 1.2 incluye en su artículo 5 un mecanismo explícito de compatibilidad con diversas licencias libres y de código abierto recíprocas enumeradas en su Apéndice.[^1]

El Apéndice incluye GPL v2/v3, AGPL v3, LGPL v2.1/v3, MPL 2.0, EPL 1.0, CeCILL 2.0/2.1, OSL 2.1/3.0, CC BY-SA 3.0 para obras que no sean software, EUPL 1.1/1.2 y LiLiQ-R/R+.[^1]

La Obra original cubierta por la EUPL continúa bajo la EUPL. El mecanismo de compatibilidad existe para hacer posible la distribución de obras combinadas más amplias cuando, de otro modo, las obligaciones de las licencias aplicables podrían entrar en conflicto.

### Uso a través de redes

La definición de "Distribución o Comunicación" de la EUPL incluye expresamente hacer que la Obra esté disponible en línea o proporcionar a otra persona acceso a sus funcionalidades esenciales.[^1] Por eso, la orientación oficial sobre la EUPL describe la EUPL 1.2 como aplicable a SaaS y a la distribución a través de redes.[^2]

Esto significa que su reciprocidad no se limita solamente a la distribución tradicional de copias del código fuente o de ejecutables.

### Multilingüe por diseño

La EUPL 1.2 tiene **23 versiones lingüísticas oficiales con el mismo valor jurídico**. El artículo 13 establece expresamente que las versiones lingüísticas aprobadas por la Comisión Europea tienen idéntico valor y que las partes pueden utilizar la versión que elijan.[^1][^5]

Esto es especialmente valioso para Cosmeron: la accesibilidad debería aplicarse no solamente al código fuente y a la documentación, sino también a la licencia bajo la cual se comparte el proyecto.

### Cadena de autoría

El artículo 6 contiene una garantía de cadena de autoría: el Licenciante original garantiza que los derechos de autor correspondientes le pertenecen o le han sido licenciados y que tiene autoridad para otorgar la licencia; cada Contribuidor presta la misma garantía respecto de sus modificaciones.[^1]

Interoperable Europe describe esta característica como un **Developer Certificate of Origin (DCO)**.[^2] Esto no debe confundirse con un flujo de contribución del repositorio que exija una línea `Signed-off-by:` en cada commit; Cosmeron puede adoptar ese procedimiento por separado si así se decide.

### Garantía y responsabilidad

La EUPL no depende de una exclusión absoluta de responsabilidad. Los artículos 7 y 8 establecen la ausencia de garantía en modalidad "as is" y una exclusión de responsabilidad con excepciones expresas para conducta dolosa, daños causados directamente a personas físicas y las normas legales sobre responsabilidad por productos, cuando resulten aplicables.[^1]

El FAQ oficial de la EUPL explica el criterio detrás de esta redacción: según el derecho europeo y el de los Estados miembros, una exclusión total de responsabilidad puede ser inválida, por lo que la EUPL utiliza una exclusión más matizada, destinada a operar dentro de las normas imperativas aplicables.[^6]

### Fundamento jurídico europeo

La EUPL 1.2 fue adoptada mediante la **Decisión de Ejecución (UE) 2017/863 de la Comisión, de 18 de mayo de 2017**. La Decisión indica que la versión 1.2 fue adoptada para facilitar aún más el intercambio y la reutilización, ampliar el alcance, aclarar las disposiciones sobre ley aplicable y jurisdicción, y actualizar el apéndice de licencias compatibles.[^7]

La propia licencia establece reglas de jurisdicción y ley aplicable en los artículos 14 y 15. Para licenciantes situados fuera de la Unión Europea, el artículo 15 prevé la aplicación del derecho belga, salvo que las partes acuerden específicamente otra cosa.[^1]

Aun así, puede ser utilizada por proyectos y desarrolladores fuera de la Unión Europea.

## Nuestra intención

Cosmeron busca equilibrar tres principios:

    libertad para usar
        +
    libertad para integrar
        +
    reciprocidad en las mejoras
        =
    desarrollo abierto sostenible

La EUPL 1.2 encaja bien con ese equilibrio para una biblioteca modular como Cosmeron.

## Referencias jurídicas y oficiales

[^1]: [Licencia Pública de la Unión Europea v1.2 — texto oficial](https://interoperable-europe.ec.europa.eu/licence/european-union-public-licence-version-12-eupl), especialmente los artículos 1, 5–8 y 13–15 y el Apéndice.
[^2]: [Introducción a la EUPL](https://interoperable-europe.ec.europa.eu/collection/eupl/introduction-eupl-licence), Interoperable Europe Portal.
[^3]: [Directiva 2009/24/CE sobre la protección jurídica de programas de ordenador](https://eur-lex.europa.eu/eli/dir/2009/24/oj), especialmente los considerandos 10 y 15 y el artículo 6.
[^4]: [Matriz de licencias de código abierto compatibles con la EUPL](https://interoperable-europe.ec.europa.eu/collection/eupl/matrix-eupl-compatible-open-source-licences), Interoperable Europe Portal.
[^5]: [Textos oficiales de la EUPL 1.2 en 23 lenguas de la UE](https://interoperable-europe.ec.europa.eu/collection/eupl/eupl-text-eupl-12).
[^6]: [FAQ de la EUPL — exclusión de responsabilidad](https://interoperable-europe.ec.europa.eu/collection/eupl/faqs), Interoperable Europe Portal.
[^7]: [Decisión de Ejecución (UE) 2017/863 de la Comisión](https://eur-lex.europa.eu/eli/dec_impl/2017/863/oj), Diario Oficial de la Unión Europea.

Para conocer los términos jurídicos, consultá siempre el texto oficial de la EUPL 1.2 y el archivo [LICENSE](../../../../LICENSE) de este repositorio.
