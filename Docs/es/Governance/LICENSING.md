# Por qué EUPL 1.2?

[Português (Brasil)](../Translations/Portugues-Brasil/Governance/LICENSING.md) · [Español (Latinoamérica)](../Translations/Espanol-LATAM/Governance/LICENSING.md)

Cosmeron está licenciado exclusivamente bajo la **European Union Public Licence, versión 1.2 (EUPL-1.2)**.

Este documento explica por qué Cosmeron eligió la EUPL. Su contenido es únicamente informativo y no modifica, sustituye ni complementa los términos de [LICENSE](../../LICENSE).

## ¿Por qué EUPL?

### Una licencia para el proyecto

La EUPL 1.2 se aplica a la "Obra" (the Work), es decir, a la Obra Original o a sus Obras Derivadas, y la licencia trata expresamente esa Obra como una obra o software, en lugar de limitar su alcance únicamente al software ejecutable.[^1]

Esto la hace adecuada para el código fuente de Cosmeron y otros materiales protegidos por derechos de autor distribuidos como parte del proyecto, como la documentación y el material técnico. La orientación oficial de la EUPL también describe la licencia como aplicable al software y a los datos auxiliares.[^2]

### Reciprocidad sin aislar la biblioteca

Cosmeron está diseñado para ser útil como biblioteca dentro de proyectos más grandes, incluidos proyectos distribuidos bajo distintos modelos de licencia.

La EUPL fue diseñada teniendo en cuenta la interoperabilidad. La orientación oficial de la EUPL explica que las interfaces, APIs y estructuras de datos pueden reutilizarse para implementar enlazado estático o dinámico con componentes independientes sin cambiar automáticamente la licencia de esos componentes independientes.[^2]

Esta orientación se basa, en particular, en la **Directiva 2009/24/CE relativa a la protección jurídica de los programas de ordenador**. El considerando 10 describe la interoperabilidad como la capacidad de los componentes de software y hardware para intercambiar y utilizar mutuamente información; el considerando 15 y el artículo 6 establecen una excepción limitada de interoperabilidad cuando la reproducción o traducción del código es indispensable para obtener la información necesaria para que un programa creado independientemente pueda interoperar con otros programas.[^3]

La orientación de compatibilidad de la EUPL parte de esas disposiciones al tratar el enlazado. También señala que no existe jurisprudencia específica del Tribunal de Justicia que resuelva definitivamente la cuestión del enlazado; por ello, Cosmeron lo considera una interpretación y orientación oficial de la EUPL, no una resolución judicial.[^4]

Al mismo tiempo, cuando se distribuyen o comunican al público modificaciones cubiertas por la EUPL u obras derivadas, el artículo 5 conserva las obligaciones recíprocas relativas a la Obra cubierta y exige que el código fuente se proporcione o se ponga a disposición gratuitamente.[^1]

En la práctica, el objetivo del proyecto es:

    Usar Cosmeron libremente.
    Integrar Cosmeron libremente.
    Mejorar Cosmeron libremente.
    Compartir las mejoras distribuidas de Cosmeron.

### Proteger las mejoras compartidas

La naturaleza recíproca de la EUPL ayuda a mantener disponibles bajo términos abiertos las mejoras distribuidas y las obras derivadas de la Obra cubierta. El artículo 5 exige que la distribución o comunicación de la Obra cubierta o de las Obras Derivadas permanezca bajo la EUPL, salvo que se aplique el mecanismo de compatibilidad.[^1]

Para Cosmeron, esto respalda una base técnica compartida y, al mismo tiempo, permite que la biblioteca participe en sistemas más grandes con licencias diferentes.

### Compatibilidad

La EUPL 1.2 incluye un mecanismo explícito de compatibilidad en el artículo 5 para licencias recíprocas de software libre y de código abierto enumeradas en su Apéndice.[^1]

El Apéndice incluye GPL v2/v3, AGPL v3, LGPL v2.1/v3, MPL 2.0, EPL 1.0, CeCILL 2.0/2.1, OSL 2.1/3.0, CC BY-SA 3.0 para obras que no sean software, EUPL 1.1/1.2 y LiLiQ-R/R+.[^1]

La Obra original cubierta por la EUPL permanece bajo la EUPL. El mecanismo de compatibilidad existe para permitir la distribución de obras combinadas más grandes cuando las obligaciones de las licencias aplicables entrarían en conflicto.

### Uso en red

La definición de "Distribución o Comunicación" de la EUPL incluye expresamente poner la Obra a disposición en línea o proporcionar a otra persona acceso a sus funcionalidades esenciales.[^1] Por ello, la orientación oficial de la EUPL describe la EUPL 1.2 como aplicable a SaaS y a la distribución en red.[^2]

Esto significa que su reciprocidad no se limita a la distribución tradicional de copias del código fuente o de ejecutables.

### Multilingüe por diseño

La EUPL 1.2 tiene **23 versiones lingüísticas oficiales con igual valor jurídico**. El artículo 13 establece expresamente que las versiones lingüísticas aprobadas por la Comisión Europea tienen idéntico valor y que las partes pueden utilizar la versión que elijan.[^1][^5]

Esto es importante para Cosmeron porque la accesibilidad debe abarcar no solo el código fuente y la documentación, sino también la licencia bajo la cual se comparte el proyecto.

### Cadena de autoría

El artículo 6 contiene una garantía relativa a la cadena de autoría: el Licenciante original garantiza que los derechos de autor pertinentes le pertenecen o le han sido licenciados y que tiene autoridad para conceder la licencia; cada Contribuyente ofrece la misma garantía para sus modificaciones.[^1]

Interoperable Europe describe esta característica como un **Developer Certificate of Origin (DCO)**.[^2] No debe confundirse con un workflow de repositorio que exija una línea `Signed-off-by:` en cada commit; Cosmeron puede adoptar por separado ese flujo de contribución si lo desea.

### Garantía y responsabilidad

La EUPL no se basa en una exención absoluta de responsabilidad. Los artículos 7 y 8 establecen la exclusión de garantía "tal cual" ("as is") y una exclusión de responsabilidad con excepciones expresas para conducta dolosa, daños causados directamente a personas físicas y normas legales de responsabilidad por productos, cuando correspondan.[^1]

El FAQ oficial de la EUPL explica la política detrás de esta redacción: según el derecho europeo y el de los Estados miembros, una exclusión total de responsabilidad puede ser inválida; por ello, la EUPL utiliza una exclusión más matizada, destinada a operar dentro de la legislación imperativa aplicable.[^6]

### Fundamento jurídico europeo

La EUPL 1.2 fue adoptada mediante la **Decisión de Ejecución (UE) 2017/863 de la Comisión, de 18 de mayo de 2017**. La Decisión indica que la versión 1.2 se adoptó para facilitar aún más el intercambio y la reutilización, ampliar el alcance, aclarar las disposiciones sobre ley aplicable y jurisdicción y actualizar el apéndice de licencias compatibles.[^7]

La propia licencia especifica las reglas de jurisdicción y ley aplicable en los artículos 14 y 15. Para los licenciantes de fuera de la Unión Europea, el artículo 15 prevé la aplicación de la ley belga, salvo que las partes acuerden específicamente otra cosa.[^1]

Aun así, puede ser utilizada por proyectos y desarrolladores de fuera de la Unión Europea.

## Nuestra intención

Cosmeron busca equilibrar tres principios:

    libertad de uso
        +
    libertad de integración
        +
    reciprocidad para las mejoras
        =
    desarrollo abierto sostenible

La EUPL 1.2 encaja bien con ese equilibrio para una biblioteca modular como Cosmeron.

## Referencias legales y oficiales

[^1]: [European Union Public Licence v1.2 — texto oficial de la licencia](https://interoperable-europe.ec.europa.eu/licence/european-union-public-licence-version-12-eupl), especialmente los artículos 1, 5–8 y 13–15 y el Apéndice.
[^2]: [Introduction to the EUPL](https://interoperable-europe.ec.europa.eu/collection/eupl/introduction-eupl-licence), Interoperable Europe Portal.
[^3]: [Directiva 2009/24/CE relativa a la protección jurídica de los programas de ordenador](https://eur-lex.europa.eu/eli/dir/2009/24/oj), especialmente los considerandos 10 y 15 y el artículo 6.
[^4]: [Matrix of EUPL compatible open source licences](https://interoperable-europe.ec.europa.eu/collection/eupl/matrix-eupl-compatible-open-source-licences), Interoperable Europe Portal.
[^5]: [Textos oficiales de la EUPL 1.2 en los 23 idiomas de la UE](https://interoperable-europe.ec.europa.eu/collection/eupl/eupl-text-eupl-12).
[^6]: [EUPL FAQ — cláusula de exclusión de responsabilidad](https://interoperable-europe.ec.europa.eu/collection/eupl/faqs), Interoperable Europe Portal.
[^7]: [Decisión de Ejecución (UE) 2017/863 de la Comisión](https://eur-lex.europa.eu/eli/dec_impl/2017/863/oj), Diario Oficial de la Unión Europea.

Para los términos legales, consulta siempre el texto oficial de la EUPL 1.2 y la [LICENSE](../../LICENSE) de este repositorio.